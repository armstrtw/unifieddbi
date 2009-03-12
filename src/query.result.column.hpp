#ifndef QUERY_RESULT_COLUMN_HPP
#define QUERY_RESULT_COLUMN_HPP

#include <Rinternals.h>

class QueryResultColumn {
protected:
  QueryResultColumn(const int position): position_(position) {}
  const int position_;
public:
  virtual const char* getName() const;
  virtual SEXP allocateSEXP(const R_len_t nrows) const;
  virtual void setValue(SEXP x, const R_len_t row) const;
};

#endif // QUERY_RESULT_COLUMN_HPP
