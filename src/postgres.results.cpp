#include <vector>
#include <string>
#include <iostream>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "postgres.results.hpp"
#include "postgres.results.column.hpp"

using std::cout;
using std::endl;

PostgresResults::PostgresResults(const PGresult *res) : QueryResults(), res_(res) {
  cout << "PostgresResults::PostgresResults(const PGresult *res)" << endl;
  initColumns();
}

PostgresResults::PostgresResults(PGresult *res) : QueryResults(), res_(res) {
  cout << "PostgresResults::PostgresResults(PGresult *res)" << endl;
  initColumns();
}

PostgresResults::~PostgresResults() {
  cout << "PostgresResults::~PostgresResults" << endl;
  PQclear(const_cast<PGresult*>(res_));
}

void PostgresResults::getStatus() const {
  cout << "status: " << PQresStatus(PQresultStatus(res_)) << endl;
  const char* err = PQresultErrorMessage(res_);
  if(err) {
    cout << "error msg: " << err << endl;
  }
}

void PostgresResults::initColumns() {
  const int ncols = ncol();
  for(int i = 0; i < ncols; i++) {
    queryResultColumns_.push_back(PostgresResultColumn::createPostgresColumn(res_, i));
  }
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
