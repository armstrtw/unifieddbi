#ifndef QUERY_RESULTS_HPP
#define QUERY_RESULTS_HPP

#include <string>
#include <vector>
#include <Rinternals.h>

#include "query.results.column.hpp"
#include "column.factory.hpp"

class QueryResults {
private:
  ColumnFactory* columnFactory_;
  int completedRows_;
  std::vector<QueryResultColumn*> queryResultColumns_;
public:
  QueryResults(ColumnFactory* columnFactory);
  virtual ~QueryResults();
  virtual void getStatus() const = 0;
  virtual bool valid() const = 0;
  virtual int nrow() const = 0;

  // implemented in base class
  const int ncol() const;
  void getColnames(std::vector<std::string>& ans) const;
  SEXP allocSEXP(const R_len_t fetch_rows) const;
  SEXP fetch(const int nrows);

};


#endif // QUERY_RESULTS_HPP
