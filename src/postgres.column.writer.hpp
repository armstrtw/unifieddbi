///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009  Whit Armstrong                                    //
//                                                                       //
// This program is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// This program is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with this program.  If not, see <http://www.gnu.org/licenses/>. //
///////////////////////////////////////////////////////////////////////////

#ifndef POSTGRES_COLUMN_WRITER_HPP
#define POSTGRES_COLUMN_WRITER_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <netinet/in.h>
#include <cstring>
#include <libpq-fe.h>
#define R_NO_REMAP
#include <Rinternals.h>


#include "column.for.writing.hpp"
#include "conversion.utils.hpp"

using namespace boost::gregorian;

class PostgresColumnWriter {
protected:
  const ColumnForWriting& wjob_;
  char*& dest_;
  int& len_;
  PostgresColumnWriter(const ColumnForWriting& wjob, char*& dest, int& len)
    :wjob_(wjob), dest_(dest), len_(len) {}
public:
  virtual ~PostgresColumnWriter() {}
  static PostgresColumnWriter* createPostgresColumnWriter(const Oid oid, const ColumnForWriting& wjob, char*& dest, int& paramLength);
  void update(const R_len_t row) {
    setCharPtr(row);
    setLength(row);
  }
  virtual void setCharPtr(const R_len_t row) = 0;
  virtual void setLength(const R_len_t row) = 0;
  virtual int getFormat() const = 0;
};

class bool2bool_writer : public PostgresColumnWriter {
private:
  bool bool_val;
public:
  bool2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    :PostgresColumnWriter(wjob, dest, len) { len_ = sizeof(bool); }
  void setCharPtr(const R_len_t row)  {
    int bool_for_writing = LOGICAL(wjob_.sexp)[row + wjob_.offset];
    if(bool_for_writing == NA_LOGICAL) {
      dest_ = NULL;
    } else {
      bool_val = static_cast<bool>(bool_for_writing);
      dest_ = reinterpret_cast<char*>(&bool_val);
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 1; }
};

class int2bool_writer : public PostgresColumnWriter {
private:
  bool bool_val;
public:
  int2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    :PostgresColumnWriter(wjob, dest, len) { len_ = sizeof(bool); }
  void setCharPtr(const R_len_t row)  {
    int bool_for_writing = INTEGER(wjob_.sexp)[row + wjob_.offset];
    if(bool_for_writing == NA_INTEGER) {
      dest_ = NULL;
    } else {
      bool_val = static_cast<bool>(bool_for_writing);
      dest_ = reinterpret_cast<char*>(&bool_val);
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 1; }
};

class char2bool_writer : public PostgresColumnWriter {
private:
  bool bool_val;
public:
  char2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    :PostgresColumnWriter(wjob, dest, len) {
    len_ = sizeof(int);
    dest_ = reinterpret_cast<char*>(&bool_val);
  }
  void setCharPtr(const R_len_t row)  {
    if(strcmp(CHAR(VECTOR_ELT(wjob_.sexp, row + wjob_.offset)),"TRUE")==0) {
      bool_val = true;
    } else {
      bool_val = false;
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 1; }
};

class char2char_writer : public PostgresColumnWriter {
public:
  char2char_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    : PostgresColumnWriter(wjob, dest, len) {
    len_ = 0;
  }
  void setCharPtr(const R_len_t row) {
    SEXP r_char_sexp = STRING_ELT(wjob_.sexp, row + wjob_.offset);
    if(r_char_sexp == R_NaString) {
      dest_ = NULL;
    } else {
      dest_ = const_cast<char*>(CHAR(r_char_sexp));
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 0; }
};

class factor2char_writer : public PostgresColumnWriter {
private:
  SEXP levels_;
public:
  factor2char_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    : PostgresColumnWriter(wjob, dest, len), levels_(Rf_getAttrib(wjob_.sexp,R_LevelsSymbol)) {
    len_ = 0;
  }
  void setCharPtr(const R_len_t row) {
    int position = INTEGER(wjob_.sexp)[row];
    if(position == NA_INTEGER) {
      dest_ = NULL;
    } else {
      dest_ = const_cast<char*>(CHAR(STRING_ELT(levels_,position-1)));
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 0; }
};

class double2float4_writer : public PostgresColumnWriter {
  i32_float_union x;
public:
  double2float4_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {    
    x.f = static_cast<float>(REAL(wjob_.sexp)[row + wjob_.offset]);
    // FIXME: check for truncation here
    x.i32 = ntohl(x.i32);
    dest_ = reinterpret_cast<char*>(&x.i32);
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class double2float8_writer : public PostgresColumnWriter {
  i64_double_union x;
public:
  double2float8_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
    len_ = 8;
  }

  void setCharPtr(const R_len_t row) {    
    x.d = REAL(wjob_.sexp)[row + wjob_.offset];
    x.i64 = ntohll(x.i64);
    dest_ = reinterpret_cast<char*>(&x.i64);
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class int2int4_writer : public PostgresColumnWriter {
  i32_int_union x;
public:
  int2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {
    x.i = INTEGER(wjob_.sexp)[row + wjob_.offset];
    if(x.i == NA_INTEGER) {
      dest_ = NULL;
    } else {
      x.i32 = ntohl(x.i32);
      dest_ = reinterpret_cast<char*>(&x.i32);
    }
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class bool2int4_writer : public PostgresColumnWriter {
  i32_int_union x;
public:
  bool2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {
    x.i = LOGICAL(wjob_.sexp)[row + wjob_.offset];
    if(x.i == NA_LOGICAL) {
      dest_ = NULL;
    } else {
      x.i32 = ntohl(x.i32);
      dest_ = reinterpret_cast<char*>(&x.i32);
    }
  }


  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class intdate2date_writer : public PostgresColumnWriter {
  i32_int_union i32_int;
public:
  intdate2date_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }
  void setCharPtr(const R_len_t row) {
    const date r_origin(1970,1,1);
    const date pg_origin(2000,1,1);

    if(INTEGER(wjob_.sexp)[row + wjob_.offset] == NA_INTEGER) {
      dest_ = NULL;
    } else {
      date r_date = r_origin + date_duration(INTEGER(wjob_.sexp)[row + wjob_.offset]);
      i32_int.i = static_cast<int>(date_period(pg_origin,r_date).length().days());
      i32_int.i32 = ntohl(i32_int.i32);
      dest_ = reinterpret_cast<char*>(&i32_int.i32);
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class doubledate2date_writer : public PostgresColumnWriter {
  i32_int_union i32_int;
public:
  doubledate2date_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }
  void setCharPtr(const R_len_t row) {
    const date r_origin(1970,1,1);
    const date pg_origin(2000,1,1);

    if(R_IsNA(REAL(wjob_.sexp)[row + wjob_.offset])) {
      dest_ = NULL;
    } else {
      date r_date = r_origin + date_duration(REAL(wjob_.sexp)[row + wjob_.offset]);
      i32_int.i = static_cast<int>(date_period(pg_origin,r_date).length().days());
      i32_int.i32 = ntohl(i32_int.i32);
      dest_ = reinterpret_cast<char*>(&i32_int.i32);
    }
  }
  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

#endif // POSTGRES_COLUMN_WRITER_HPP
