#ifndef QUERY_RESULT_COLUMN_HPP
#define QUERY_RESULT_COLUMN_HPP

#include <Rinternals.h>

class QueryResultColumn {
protected:
  QueryResultColumn(const int position): position_(position) {}
  const int position_;
public:
  virtual ~QueryResultColumn() {}
  virtual const char* getName() const = 0;
  virtual SEXP allocateSEXP(const R_len_t nrows) const = 0;
  virtual void setValue(SEXP x, const R_len_t row) const = 0;
};

#endif // QUERY_RESULT_COLUMN_HPP
