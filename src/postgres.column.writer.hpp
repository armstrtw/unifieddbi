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
public:
  bool2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    :PostgresColumnWriter(wjob, dest, len) { len_ = sizeof(int); }
  void setCharPtr(const R_len_t row)  {
    dest_ = reinterpret_cast<char*>(&LOGICAL(wjob_.sexp)[row + wjob_.offset]);
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 1; }
};

class int2bool_writer : public PostgresColumnWriter {
public:
  int2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    :PostgresColumnWriter(wjob, dest, len) { len_ = sizeof(int); }
  void setCharPtr(const R_len_t row)  {
    dest_ = reinterpret_cast<char*>(&INTEGER(wjob_.sexp)[row + wjob_.offset]);
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
    dest_ = const_cast<char*>(CHAR(STRING_ELT(wjob_.sexp, row + wjob_.offset)));
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 0; }
};

class factor2char_writer : public PostgresColumnWriter {
private:
  SEXP levels_;
public:
factor2char_writer(const ColumnForWriting& wjob, char*& dest, int& len)
    : PostgresColumnWriter(wjob, dest, len) {
    len_ = NULL;
  }
  void setCharPtr(const R_len_t row) {
    dest_ = const_cast<char*>(CHAR(STRING_ELT(levels_,INTEGER(wjob_.sexp)[row]-1)));
  }
  void setLength(const R_len_t row) {}
  int getFormat() const { return 0; }
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
    hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&INTEGER(wjob_.sexp)[row + wjob_.offset]));
    dest_ = reinterpret_cast<char*>(&hton_flipped_int);
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
    hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&LOGICAL(wjob_.sexp)[row + wjob_.offset]));
    dest_ = reinterpret_cast<char*>(&hton_flipped_int);
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
