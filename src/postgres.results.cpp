#include <vector>
#include <string>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "postgres.results.hpp"
#include "postgres.result.column.hpp"


PostgresResults::PostgresResults(const PGresult *res) : res_(res) {
  if(PQresultStatus(res) == PGRES_TUPLES_OK) {
    void initColumns();
  }
}

void PostgresResults::initColumns() {
  const int ncols = ncol();
  for(int i = 0; i < ncols; i++) {
    queryResultColumns_.push_back(PostgresResultColumn::createPostgresColumn(res_, i));
  }
}

PostgresResults::~PostgresResults() {
  PQclear(const_cast<PGresult*>(res_));
}

bool PostgresResults::valid() const {
  const int res_status = PQresultStatus(res_);
  if(res_status == PGRES_COMMAND_OK ||
     res_status == PGRES_TUPLES_OK) {
    return true;
  }
  return false;
}

int PostgresResults::nrow() const {
  return PQntuples(res_);
}

int PostgresResults::ncol() const {
  return PQnfields(res_);
}
