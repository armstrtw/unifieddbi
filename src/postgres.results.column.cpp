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

#include <iostream>
#include <ctime>
#include "postgres.results.column.hpp"

using std::cout;
using std::cerr;
using std::endl;

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
  return allocVector(REALSXP, nrows);
}
void defaultResultColumn::setValue(SEXP x, const R_len_t row) const {
  REAL(x)[row] = NA_REAL;
}

float_char::float_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP float_char::allocateSEXP(const R_len_t nrows) const {
  return allocVector(REALSXP, nrows);
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
  return allocVector(INTSXP, nrows);
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
  PROTECT(ans = allocVector(REALSXP, nrows));

  // create and add dates class to dates object
  SEXP r_dates_class;
  PROTECT(r_dates_class = allocVector(STRSXP, 2));
  SET_STRING_ELT(r_dates_class, 0, mkChar("POSIXt"));
  SET_STRING_ELT(r_dates_class, 1, mkChar("POSIXct"));
  classgets(ans, r_dates_class);
  UNPROTECT(2); // ans, r_dates_class
  return ans;
}

void date_char::setValue(SEXP x, const R_len_t row) const {
  struct tm tm_date;
  if(isNullValue(row)) {
    REAL(x)[row] = NA_REAL;
  } else {
    memset(&tm_date, '\0', sizeof(struct tm));
    strptime(getValue(row),"%Y-%m-%d",&tm_date);
    tm_date.tm_isdst = -1;
    REAL(x)[row] = static_cast<double>(mktime(&tm_date));
  }
}

text_char::text_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP text_char::allocateSEXP(const R_len_t nrows) const {
  return allocVector(STRSXP, nrows);
}

void text_char::setValue(SEXP x, const R_len_t row) const {
  if(isNullValue(row)) {
    SET_STRING_ELT(x,row,NA_STRING);
  } else {
    SET_STRING_ELT(x,row,mkChar(getValue(row)));
  }
}

TIMESTAMPOID_char::TIMESTAMPOID_char(const PGresult *res, const int position):
  PostgresResultColumn(res,position) {}

SEXP TIMESTAMPOID_char::allocateSEXP(const R_len_t nrows) const {
  SEXP ans;
  PROTECT(ans = allocVector(REALSXP, nrows));

  // create and add dates class to dates object
  SEXP r_dates_class;
  PROTECT(r_dates_class = allocVector(STRSXP, 2));
  SET_STRING_ELT(r_dates_class, 0, mkChar("POSIXt"));
  SET_STRING_ELT(r_dates_class, 1, mkChar("POSIXct"));
  classgets(ans, r_dates_class);
  UNPROTECT(2); // ans, r_dates_class
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
