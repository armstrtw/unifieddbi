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

#include <ctime>
#include <netinet/in.h>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/local_time/local_time_types.hpp>

#include "postgres.results.column.hpp"
#include "conversion.utils.hpp"

using namespace boost::gregorian;
using namespace postgres;

PostgresResultColumn::~PostgresResultColumn() {}

PostgresResultColumn::PostgresResultColumn(const PGresult *res, const int position):
  QueryResultColumn(position),
  res_(res) {}

const bool PostgresResultColumn::isNullValue(const int row) const {
  return static_cast<bool>(PQgetisnull(res_,row,position_));
}

const char* PostgresResultColumn::getValue(const int row) const {
  return PQgetvalue(res_,row,position_);
}

const char* PostgresResultColumn::getName() const {
  return const_cast<const char*>(PQfname(res_,position_));
}


defaultResultColumn::defaultResultColumn(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP defaultResultColumn::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(REALSXP, nrows);
}
void defaultResultColumn::setValue(SEXP x, const R_len_t row) const {
  REAL(x)[row] = NA_REAL;
}

float_char::float_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP float_char::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(REALSXP, nrows);
}
void float_char::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    REAL(x)[row] = atof(getValue(row));
  }
}

int_char::int_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP int_char::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(INTSXP, nrows);
}

void int_char::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_INTEGER;
  } else {
    INTEGER(x)[row] = atoi(getValue(row));
  }
}


date_char::date_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP date_char::allocateSEXP(const R_len_t nrows) const {
  SEXP ans;
  PROTECT(ans = Rf_allocVector(INTSXP, nrows));
  add_date_class(ans);
  UNPROTECT(1); // ans
  return ans;
}

void date_char::setValue(SEXP x, const R_len_t row) const {
  const date r_epoch(1970,1,1);
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_INTEGER;
  } else {
    date_period dp(r_epoch,from_string(std::string(getValue(row))));
    INTEGER(x)[row] = static_cast<int>(dp.length().days());
  }
}

text_char::text_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP text_char::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(STRSXP, nrows);
}

void text_char::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    SET_STRING_ELT(x,row,NA_STRING);
  } else {
    SET_STRING_ELT(x,row,Rf_mkChar(getValue(row)));
  }
}

TIMESTAMPOID_char::TIMESTAMPOID_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP TIMESTAMPOID_char::allocateSEXP(const R_len_t nrows) const {
  SEXP ans;
  PROTECT(ans = Rf_allocVector(REALSXP, nrows));
  std::string timezone("");
  add_posixct_classes(ans,timezone.c_str());
  UNPROTECT(1); // ans
  return ans;
}

void TIMESTAMPOID_char::setValue(SEXP x, const R_len_t row) const {
  struct tm tm_date;
  char* milis_char;
  double milis = 0;
  double seconds;
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    memset(&tm_date, '\0', sizeof(struct tm));
    milis_char = strptime(getValue(row),"%Y-%m-%d %H:%M:%S",&tm_date);
    tm_date.tm_isdst = -1;
    seconds = static_cast<double>(mktime(&tm_date));
    if(milis_char) {
      milis = atof(milis_char);
    }
    REAL(x)[row] = seconds + milis;
  }
}

BOOLOID_char::BOOLOID_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP BOOLOID_char::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(LGLSXP, nrows);
}

void BOOLOID_char::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_LOGICAL;
  } else {
    if(strcmp(getValue(row),"t") == 0) {
      LOGICAL(x)[row] = true;
    } else {
      LOGICAL(x)[row] = false;
    }
  }
}

INT2OID_binary::INT2OID_binary(const PGresult *res, const int position):
PostgresResultColumn(res,position) {}

SEXP INT2OID_binary::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(INTSXP, nrows);
}

void INT2OID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_INTEGER;
  } else {
    const char *from_pg = getValue(row);
    const uint16_t swap = ntohs(*reinterpret_cast<const uint16_t*>(from_pg));
    INTEGER(x)[row] = static_cast<int>(swap);
  }
}

INT4OID_binary::INT4OID_binary(const PGresult *res, const int position):
PostgresResultColumn(res,position) {}

SEXP INT4OID_binary::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(INTSXP, nrows);
}

void INT4OID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_INTEGER;
  } else {
    const char *from_pg = getValue(row);
    const uint32_t swap = ntohl(*reinterpret_cast<const uint32_t*>(from_pg));
    INTEGER(x)[row] = *reinterpret_cast<const int*>(&swap);
  }
}

INT8OID_binary::INT8OID_binary(const PGresult *res, const int position):
PostgresResultColumn(res,position) {}

SEXP INT8OID_binary::allocateSEXP(const R_len_t nrows) const {
  // use double, b/c 8 byte int can overflow R's R_int_t (32bit int)
  return Rf_allocVector(REALSXP, nrows);
}

void INT8OID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    const char *from_pg = getValue(row);
    const uint64_t swap = ntohll(*reinterpret_cast<const uint64_t*>(from_pg));
    REAL(x)[row] = static_cast<double>(*reinterpret_cast<const long*>(&swap));
  }
}

DATEOID_binary::DATEOID_binary(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP DATEOID_binary::allocateSEXP(const R_len_t nrows) const {
  SEXP ans;
  PROTECT(ans = Rf_allocVector(INTSXP, nrows));
  add_date_class(ans);
  UNPROTECT(1); // ans
  return ans;
}

void DATEOID_binary::setValue(SEXP x, const R_len_t row) const {
  const date pg_epoch(2000,Jan,1);
  const date r_epoch(1970,Jan,1);
  if(isNullValue(row)) {
    INTEGER(x)[row] = NA_INTEGER;
  } else {
    const char *from_pg = getValue(row);
    const uint32_t swap = ntohl(*reinterpret_cast<const int32_t*>(from_pg));
    const int pg_jdate = *reinterpret_cast<const int*>(&swap);
    date_duration dd(pg_jdate);
    date ans = pg_epoch + dd;
    date_period dp(r_epoch,ans);
    INTEGER(x)[row] = static_cast<int>(dp.length().days());
  }
}

FLOAT8OID_binary::FLOAT8OID_binary(const PGresult *res, const int position):
PostgresResultColumn(res,position) {}

SEXP FLOAT8OID_binary::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(REALSXP, nrows);
}
void FLOAT8OID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    i64_double_union i64_double;
    const char *from_pg = getValue(row);
    memcpy(&i64_double.i64,from_pg,sizeof(i64_double.i64));
    i64_double.i64 = ntohll(i64_double.i64);
    REAL(x)[row] = i64_double.d;
  }
}

FLOAT4OID_binary::FLOAT4OID_binary(const PGresult *res, const int position):
PostgresResultColumn(res,position) {}

SEXP FLOAT4OID_binary::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(REALSXP, nrows);
}
void FLOAT4OID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    i32_float_union i32_float;
    const char *from_pg = getValue(row);
    memcpy(&i32_float.i32,from_pg,sizeof(i32_float.i32));
    i32_float.i32 = ntohll(i32_float.i32);
    REAL(x)[row] = i32_float.f;
  }
}

BOOLOID_binary::BOOLOID_binary(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP BOOLOID_binary::allocateSEXP(const R_len_t nrows) const {
  return Rf_allocVector(LGLSXP, nrows);
}
void BOOLOID_binary::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    LOGICAL(x)[row] = NA_LOGICAL;
  } else {
    const char *from_pg = getValue(row);
    // does network order mater if we are just testing for any bits != 0
    // possibly use c++ bitset, if we really need to deal w/ bit order
    LOGICAL(x)[row] = (*from_pg) ? 1 : 0;
  }
}

TIMESTAMPOID_binary::TIMESTAMPOID_binary(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP TIMESTAMPOID_binary::allocateSEXP(const R_len_t nrows) const {
  SEXP ans;
  PROTECT(ans = Rf_allocVector(REALSXP, nrows));
  std::string timezone("");
  add_posixct_classes(ans,timezone.c_str());
  UNPROTECT(1); // ans
  return ans;
}

void TIMESTAMPOID_binary::setValue(SEXP x, const R_len_t row) const {
  const double pg_epoch = 946684800;
  const double microseconds_in_second = 1e6;
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    const char *from_pg = getValue(row);
    const uint64_t swap = ntohll(*reinterpret_cast<const uint64_t*>(from_pg));
    //cout << *reinterpret_cast<const long*>(&swap) << endl;
    REAL(x)[row] =  static_cast<double>(*reinterpret_cast<const long*>(&swap))/microseconds_in_second + pg_epoch;
  }
}
