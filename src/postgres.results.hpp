#ifndef POSTGRES_RESULTS_HPP
#define POSTGRES_RESULTS_HPP

#include <iostream>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "query.results.hpp"

using std::cout;
using std::endl;

class PostgresResults : public QueryResults {
private:
  const PGresult *res_;
  void initColumns();
public:
  PostgresResults();
  PostgresResults(const PGresult *res);
  PostgresResults(PGresult *res);
  ~PostgresResults();
  void getStatus() const;
  bool valid() const;
  int nrow() const;
  int ncol() const;
  void getColnames(std::vector<std::string>& ans) const;
};

#endif // POSTGRES_RESULTS_HPP
