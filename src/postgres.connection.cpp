///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009  Whit Armstrong                                    //
//                                                                       //
// This program is free software: you can redistribute it and/or modify  //
// it under the terms of the GNU General Public License as published by  //
// the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                   //
//                                                                       //
// This program is distributed in the hope that it will be useful,       //
// but WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
// GNU General Public License for more details.                          //
//                                                                       //
// You should have received a copy of the GNU General Public License     //
// along with this program.  If not, see <http://www.gnu.org/licenses/>. //
///////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <Rinternals.h>
#include <cstdlib>

#include <Rutilities.hpp>
#include "postgres.connection.hpp"
#include "utils.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using namespace postgres;

PostgresConnection::~PostgresConnection() {
  cout << "PostgresConnection::~PostgresConnection()" << endl;
  disConnect();
}

PostgresConnection::PostgresConnection(): conn_(static_cast<PGconn*>(NULL)) {
  cout << "PostgresConnection::PostgresConnection()" << endl;
}

void PostgresConnection::connect(const char* connectionString) {
  conn_ = PQconnectdb(connectionString);
  verifyConnection();
}

void PostgresConnection::connect(const char* user,
                                 const char* pass,
                                 const char* host,
                                 const char* port,
                                 const char* tty,
                                 const char* dbName,
                                 const char* options) {
  conn_ = PQsetdbLogin(host,port,options,tty,dbName,user,pass);
  verifyConnection();
}

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
  if(conn_) {
    PQfinish(conn_);
    conn_ = NULL;
  }
}

bool PostgresConnection::transaction_begin() {
  PostgresResults* beginResults = sendQuery("BEGIN;");
  // FIXME: check results
  return true;
}

bool PostgresConnection::rollback() {
  PostgresResults* rollbackResults = sendQuery("ROLLBACK;");
  // FIXME: check results
  return true;
}

bool PostgresConnection::commit() {
  PostgresResults* commitResults = sendQuery("COMMIT;");
  // FIXME: check results
  return true;
}

SEXP PostgresConnection::listTables() {
  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return R_NilValue;
  }

  const char* query = "select schemaname, tablename from pg_tables where schemaname !='information_schema' and schemaname !='pg_catalog'";
  QueryResults* res = sendQuery(query);
  // FIXME: convert to string vector...
  return res->fetch(-1);
}

bool PostgresConnection::existsTable(const char* tableName_char) {
  stringstream query;
  string tableName(tableName_char);
  string schemaName("public");

  if(!connectionValid()) {
    cerr << "cannot connect to databse" << endl;
    return false;
  }

  // find schema name
  string::size_type pos = tableName.find(".");
  if(pos != string::npos && pos != tableName.size()) {
    schemaName = tableName.substr(0, pos);
    tableName = tableName.substr(pos + 1, tableName.size());
    cout << schemaName << endl;
    cout << tableName << endl;
  }
  query << "select count(*) from pg_tables where schemaname = '" << schemaName <<  "' and tablename = '" << tableName << "'";
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

PostgresResults* PostgresConnection::sendQuery(const char* query) {
  if(query && connectionValid()) {
    //return new PostgresResults(static_cast<const PGresult*>(PQexec(conn_,query)));
    return new PostgresResults(static_cast<const PGresult*>(PQexecParams(conn_,query,0,NULL,NULL,NULL,NULL,1)));
  }
  cerr << "bad connection to database" << endl;
  return static_cast<PostgresResults*>(NULL);
}
