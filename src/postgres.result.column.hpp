#ifndef POSTGRES_RESULT_COLUMN_HPP
#define POSTGRES_RESULT_COLUMN_HPP

#include <cstring>
#include <cstdlib>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "query.result.column.hpp"

class PostgresResultColumn : public QueryResultColumn {
protected:
  const PGresult *res_;
  PostgresResultColumn(const PGresult *res, const int position);
  const bool isNullValue(const int row) const;
  const char* getValue(const int row) const;
public:  
  virtual SEXP allocateSEXP(const R_len_t nrows) const;
  virtual void setValue(SEXP x, const R_len_t row) const;
  const char* getName() const;
  static PostgresResultColumn* createPostgresColumn(const PGresult *res, const int position);
};


class defaultResultColumn : public PostgresResultColumn {
  defaultResultColumn(const PGresult *res, const int position);
  friend class PostgresResultColumn;
public:
  SEXP allocateSEXP(const R_len_t nrows) const;
  void setValue(SEXP x, const R_len_t row) const;
};


// should take care of
// FLOAT4OID
// FLOAT8OID
class float_char : public PostgresResultColumn {
  float_char(const PGresult *res, const int position);
  friend class PostgresResultColumn;
public:
  SEXP allocateSEXP(const R_len_t nrows) const;
  void setValue(SEXP x, const R_len_t row) const;
};

// should take care of
// INT8OID
// INT2OID
class int_char : public PostgresResultColumn {
  int_char(const PGresult *res, const int position);
  friend class PostgresResultColumn;
public:
  SEXP allocateSEXP(const R_len_t nrows) const;
  void setValue(SEXP x, const R_len_t row) const;
};


// class BOOLOID_char : public PostgresResultColumn {
//   BOOLOID_char(const PGresult *res, const int position):
//     PostgresResultColumn(res,position) {}
//   friend class PostgresResultColumn;
// public: 
//   SEXP allocateSEXP(const R_len_t nrows) {
//     return allocVector(LGLSXP, nrows);
//   }
//   void setValue(SEXP x, const R_len_t row) {
//     if(isNull(row)) {
//       LOGICAL(x)[row] = NA_LOGICAL;
//     } else {
//       if(strcmp(getValue(row),"t") == 0) {
//         LOGICAL(x)[row] = true;
//       } else {
//         LOGICAL(x)[row] = false;
//       }
//     }
//   }
// };

#endif //POSTGRES_RESULT_COLUMN_HPP
