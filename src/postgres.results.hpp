#ifndef POSTGRES_RESULTS_HPP
#define POSTGRES_RESULTS_HPP

#include <libpq-fe.h>
#include <Rinternals.h>

#include "query.results.hpp"

class PostgresResults : public QueryResults {
private:
  const PGresult *res_;
  void initColumns();
public:
  PostgresResults(const PGresult *res);
  ~PostgresResults();
  bool valid() const;
  int nrow() const;
  int ncol() const;
  void getColnames(std::vector<std::string>& ans) const;
};

#endif // POSTGRES_RESULTS_HPP
