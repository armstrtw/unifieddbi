#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <Rinternals.h>
#include <cstdlib>

#include "postgres.connection.hpp"

using std::cout;
using std::cerr;
using std::endl;

PostgresConnection::~PostgresConnection() {
  cout << "PostgresConnection::~PostgresConnection()" << endl;
  PQfinish(conn_);
  conn_ = NULL;
}

PostgresConnection::PostgresConnection(): conn_(static_cast<PGconn*>(NULL)) {}

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
  return true;
}

bool PostgresConnection::rollback() {
  // does nothing on Postgres
  return false;
}

SEXP PostgresConnection::listTables() {
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return R_NilValue;
  }

  const char* query = "select tablename from pg_tables where schemaname !='information_schema' and schemaname !='pg_catalog'";
  QueryResults* res = sendQuery(query);
  // FIXME: convert to string vector...
  return res->fetch(-1);
}

SEXP PostgresConnection::readTable(const char* tableName) {
  std::stringstream query;

  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return R_NilValue;
  }
  query << "select * from " << tableName << endl;
  QueryResults* res = sendQuery(query.str().c_str());

  return res->fetch(-1);
}

int PostgresConnection::writeTable(const char* tableName, SEXP x, const bool writeRowNames, const bool overwrite, const bool append) {
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
  }
  return 0;
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
  //std::stringstream query;
  //query <<  "drop table " << tableName;
  //PostgresResults* res = sendQuery(query.str().c_str());
  // FIXME: send total rows affected
  cerr << "not implemented yet" << endl;
  return false;
}

PostgresResults* PostgresConnection::sendQuery(const char* query) {
  if(query && connectionValid()) {
    return new PostgresResults(static_cast<const PGresult*>(PQexec(conn_,query)));
  }
  cerr << "bad connection to database" << endl;
  return static_cast<PostgresResults*>(NULL);
}
