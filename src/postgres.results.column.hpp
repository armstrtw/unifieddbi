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

#ifndef POSTGRES_RESULTS_COLUMN_HPP
#define POSTGRES_RESULTS_COLUMN_HPP

#include <cstring>
#include <cstdlib>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "query.results.column.hpp"

namespace postgres {

  class PostgresResultColumn : public QueryResultColumn {
  protected:
    const PGresult *res_;
    PostgresResultColumn(const PGresult *res, const int position);
    const bool isNullValue(const int row) const;
    const char* getValue(const int row) const;
  public:
    virtual ~PostgresResultColumn();
    const char* getName() const;
    virtual SEXP allocateSEXP(const R_len_t nrows) const = 0;
    virtual void setValue(SEXP x, const R_len_t row) const = 0;
  };

  // use this one when we can't find or haven't implemented
  // a postgres OID type
  class defaultResultColumn : public PostgresResultColumn {
    defaultResultColumn(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };


  // should take care of
  // FLOAT4OID
  // FLOAT8OID
  class float_char : public PostgresResultColumn {
    float_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  // should take care of
  // INT8OID
  // INT2OID
  class int_char : public PostgresResultColumn {
    int_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };


  // DATEOID
  class date_char : public PostgresResultColumn {
    date_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  // TEXTOID
  class text_char : public PostgresResultColumn {
    text_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class TIMESTAMPOID_char : public PostgresResultColumn {
    TIMESTAMPOID_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class BOOLOID_char : public PostgresResultColumn {
    BOOLOID_char(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class INT4OID_binary : public PostgresResultColumn {
    INT4OID_binary(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class DATEOID_binary : public PostgresResultColumn {
    DATEOID_binary(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class FLOAT8OID_binary : public PostgresResultColumn {
    FLOAT8OID_binary(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };

  class BOOLOID_binary : public PostgresResultColumn {
    BOOLOID_binary(const PGresult *res, const int position);
    friend class PostgresColumnFactory;
  public:
    SEXP allocateSEXP(const R_len_t nrows) const;
    void setValue(SEXP x, const R_len_t row) const;
  };


} // namespace postgres
#endif //POSTGRES_RESULTS_COLUMN_HPP
