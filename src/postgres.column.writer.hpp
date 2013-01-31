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
#include <iostream>

#include <netinet/in.h>
#include <cstring>
#include <libpq-fe.h>
#include <Rinternals.h>


#include "column.for.writing.hpp"
#include "conversion.utils.hpp"

class PostgresColumnWriter {
protected:
  const ColumnForWriting& wjob_;
  char*& dest_;
  int& len_;
  PostgresColumnWriter(const ColumnForWriting& wjob, char*& dest, int& len)
    :wjob_(wjob), dest_(dest), len_(len) {}
public:
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
    len_ = NULL;
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
    : PostgresColumnWriter(wjob, dest, len), levels_(getAttrib(wjob_.sexp,R_LevelsSymbol)) {
    len_ = NULL;
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
  uint32_t hton_flipped_float;
public:
  double2float4_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {
    float float_for_writing = static_cast<float>(REAL(wjob_.sexp)[row + wjob_.offset]);
    // FIXME: check for truncation here
    hton_flipped_float = ntohl(*reinterpret_cast<uint32_t*>(&float_for_writing));
    dest_ = reinterpret_cast<char*>(&hton_flipped_float);
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class double2float8_writer : public PostgresColumnWriter {
  uint64_t hton_flipped_double;
public:
  double2float8_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
    len_ = 8;
  }

  void setCharPtr(const R_len_t row) {
    hton_flipped_double = ntohll(*reinterpret_cast<uint64_t*>(&REAL(wjob_.sexp)[row + wjob_.offset]));
    dest_ = reinterpret_cast<char*>(&hton_flipped_double);
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class int2int4_writer : public PostgresColumnWriter {
  uint32_t hton_flipped_int;
public:
  int2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {
    int int_for_writing = INTEGER(wjob_.sexp)[row + wjob_.offset];
    if(int_for_writing == NA_INTEGER) {
      dest_ = NULL;
    } else {
      hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&int_for_writing));
      dest_ = reinterpret_cast<char*>(&hton_flipped_int);
    }
  }

  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

class bool2int4_writer : public PostgresColumnWriter {
  uint32_t hton_flipped_int;
public:
  bool2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row) {
    int bool_for_writing = LOGICAL(wjob_.sexp)[row + wjob_.offset];
    if(bool_for_writing == NA_LOGICAL) {
      dest_ = NULL;
    } else {
      hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&bool_for_writing));
      dest_ = reinterpret_cast<char*>(&hton_flipped_int);
    }
  }


  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};


class posixct2date_writer : public PostgresColumnWriter {
  //const double postgres_epoch_diff = 946684800;
  uint32_t hton_flipped_int;
public:
  posixct2date_writer(const ColumnForWriting& wjob, char*& dest, int& len)  : PostgresColumnWriter(wjob, dest, len) {
    len_ = 4;
  }

  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row) {}
  int getFormat() const {
    return 1;
  }
};

#endif // POSTGRES_COLUMN_WRITER_HPP
