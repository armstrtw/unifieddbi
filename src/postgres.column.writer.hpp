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

#include <libpq-fe.h>
#include <Rinternals.h>

#include "column.for.writing.hpp"

class PostgresColumnWriter {
protected:
  const ColumnForWriting& wjob_;
  char*& dest_;
  int& len_;
  PostgresColumnWriter(const ColumnForWriting& wjob, char*& dest, int& len);
public:
  static PostgresColumnWriter* createPostgresColumnWriter(const Oid oid, const ColumnForWriting& wjob, char*& dest, int& paramLength);
  void update(const R_len_t row);
  virtual void setCharPtr(const R_len_t row) = 0;
  virtual void setLength(const R_len_t row) = 0;
  virtual int getFormat() const = 0;
};

class bool2bool_writer : public PostgresColumnWriter {
public:
  bool2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len);
  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row);
  int getFormat() const;
};

class char2char_writer : public PostgresColumnWriter {
public:
  char2char_writer(const ColumnForWriting& wjob, char*& dest, int& len);
  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row);
  int getFormat() const;
};

class factor2char_writer : public PostgresColumnWriter {
private:
  SEXP levels_;
public:
  factor2char_writer(const ColumnForWriting& wjob, char*& dest, int& len);
  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row);
  int getFormat() const;
};

class double2float8_writer : public PostgresColumnWriter {
  uint64_t hton_flipped_double;
public:
  double2float8_writer(const ColumnForWriting& wjob, char*& dest, int& len);
  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row);
  int getFormat() const;
};

class int2int4_writer : public PostgresColumnWriter {
  uint32_t hton_flipped_int;
public:
  int2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len);
  void setCharPtr(const R_len_t row);
  void setLength(const R_len_t row);
  int getFormat() const;
};

#endif // POSTGRES_COLUMN_WRITER_HPP
