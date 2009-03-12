#include <iostream>
#include "postgres.result.column.hpp"
#include "postgres.oid.hpp"

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

PostgresResultColumn* PostgresResultColumn::createPostgresColumn(const PGresult *res, const int position) {
  switch(static_cast<pg_oidT>(PQftype(res,position))) {
  case INT4OID:
  case INT8OID:
  case OIDOID:
    return new int_char(res, position);
  case FLOAT4OID:
  case FLOAT8OID:
    return new float_char(res, position);
  default:
    cerr << "conversion not supported using null conversion for column number: " << position << endl;
    return new defaultResultColumn(res, position);
  }
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

