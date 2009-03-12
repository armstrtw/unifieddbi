#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <Rinternals.h>
#include <cstdlib>

#include "postgres.connection.hpp"


using std::cerr;
using std::endl;

void PostgresConnection::connect(const char* connectionString) {
  conn_ = PQconnectdb(connectionString);
  verifyConnection();
}

// not implemented yet
////////////////////////////////////////////////////
// void PostgresConnection::connect(const char* user,
//                                  const char* pass,
//                                  const char* host,
//                                  const char* port,
//                                  const char* tty,
//                                  const char* dbName,
//                                  const char* options) {
//   conn_ = PQsetdbLogin(host,port,options,tty,databaseName,user,pass);
//   verifyConnection();
// }

bool PostgresConnection::connectionValid() {
  if(conn_ && PQstatus(conn_) == CONNECTION_OK) {
    return true;
  } else {
    return false;
  }
}

void PostgresConnection::verifyConnection() {
  if(PQstatus(conn_) != CONNECTION_OK) {
    cerr << PQerrorMessage(conn_) << endl;
    PQfinish(conn_);
    // throw
    conn_ = NULL;
  }
}

void PostgresConnection::disConnect() {
  PQfinish(conn_);
  conn_ = NULL;
}

bool PostgresConnection::commit() {
  // does nothing on Postgres
}

bool PostgresConnection::rollback() {
  // does nothing on Postgres
}

void PostgresConnection::listTables(std::vector<std::string>& ans) {
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return;
  }

  const char* query = "select tablename from pg_tables where schemaname !='information_schema' and schemaname !='pg_catalog'";
  QueryResults* res = sendQuery(query);
  // FIXME: convert to string vector...
}

SEXP PostgresConnection::readTable(const char* tableName) {
  std::stringstream query;

  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return R_NilValue;
  }
  query << "select * from " << tableName << endl;
  QueryResults* res = sendQuery(query.str().c_str());

  //FIXME: convert to SEXP
}

void PostgresConnection::writeTable(const char* tableName, SEXP x, const bool writeRowNames, const bool overwrite, const bool append) {
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
  }

}

bool PostgresConnection::existsTable(const char* tableName) {
  std::stringstream query;
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return false;
  }

  query << "select count(*) from pg_tables where tablename = '" << tableName << "'";
  PGresult* res = PQexec(conn_,query.str().c_str());

  if(PQresultStatus(res)==PGRES_TUPLES_OK && PQntuples(res)) {
    // FIXME: assuming integer return value
    // add check for OID value
    if(atoi(PQgetvalue(res, 0, 0)) == 1) {
      return true;
    }
  }
  return false;
}

bool PostgresConnection::removeTable(const char* tableName) {
  // FIXME: check for single word or schema.table ie no spaces?
  std::stringstream query;
  query <<  "drop table " << tableName;
  QueryResults* res = sendQuery(query.str().c_str());
  // FIXME: send total rows affected
}

PostgresResults* PostgresConnection::sendQuery(const char* query) {
  PostgresResults* ans = NULL;
  if(query && connectionValid()) {
    ans = new PostgresResults(PQexec(conn_,query));
  } else {
    return NULL;
  }
}
