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
#include <algorithm>
#include <Rinternals.h>
#include <cstdlib>

#include "postgres.connection.hpp"
#include "postgres.column.writer.hpp"
#include "utils.hpp"
#include "database.errors.hpp"

using std::vector;
using std::string;
using std::stringstream;
using namespace postgres;

PostgresConnection::~PostgresConnection() {
  disConnect();
}

PostgresConnection::PostgresConnection(): conn_(static_cast<PGconn*>(NULL)) {
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
    REprintf(PQerrorMessage(conn_));
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
  PostgresResults* res = sendQuery("BEGIN;");
  if(res->valid()) {
    return true;
  }
  return false;
}

bool PostgresConnection::rollback() {
  PostgresResults* res = sendQuery("ROLLBACK;");
  if(res->valid()) {
    return true;
  }
  return false;
}

bool PostgresConnection::commit() {
  PostgresResults* res = sendQuery("COMMIT;");
  if(res->valid()) {
    return true;
  }
  return false;
}

SEXP PostgresConnection::listTables() {
  if(!connectionValid()) {
    REprintf("cannot connect to databse");
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
    REprintf("cannot connect to database");
    return false;
  }

  // find schema name
  string::size_type pos = tableName.find(".");
  if(pos != string::npos && pos != tableName.size()) {
    schemaName = tableName.substr(0, pos);
    tableName = tableName.substr(pos + 1, tableName.size());
  }
  query << "select count(*) from pg_tables where schemaname = '" << schemaName <<  "' and tablename = '" << tableName << "'";
  PGresult* res = PQexec(conn_,query.str().c_str());

  if(PQresultStatus(res)==PGRES_TUPLES_OK && PQntuples(res)) {
    int nrows = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    if(nrows == 1) {
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
  REprintf("bad connection to database");
  return static_cast<PostgresResults*>(NULL);
}

void PostgresConnection::getOIDs(const char* tableName, vector<string>& relationNames, vector<Oid>& oids) {
  stringstream query;
  query << "SELECT attname, atttypid FROM pg_attribute WHERE attrelid = '" << tableName << "'::regclass and attnum > 0 and attisdropped = FALSE";
  PGresult* res = PQexec(conn_, query.str().c_str());
  if(PQresultStatus(res)==PGRES_TUPLES_OK && PQntuples(res)) {
    for(int i = 0; i < PQntuples(res); i++) {
      relationNames.push_back(PQgetvalue(res, i, 0));
      oids.push_back(atoi(PQgetvalue(res, i, 1)));
    }
  }
  PQclear(res);
}

// FIXME: look up colnames to make sure we have proper order
// and only write the columns that we match
string PostgresConnection::genInsertQuery(const char* tableName, const vector<ColumnForWriting>& cols) {
  int ncols = cols.size();
  stringstream query;
  query << "INSERT INTO " << tableName;
  query << " (";
  for(int i = 0; i < ncols; i++) {
    query << cols[i].colname;
    if(i < ncols-1) {
      query << ",";
    }
  }
  query << ") VALUES (";
  for(int i = 0; i < ncols; i++) {
    query << "$" << i + 1;
    if( i < ncols-1) {
      query << ",";
    }
  }
  query << ")";
  return query.str();
}

// walk through cols and look up wich colname matches in the DB
void PostgresConnection::find_column_oids(vector<Oid>& oids, const char* tableName, const vector<ColumnForWriting>& cols) {
  size_t offset = 0;
  vector<string> relationNames;
  vector<Oid> table_oids;
  vector<string>::iterator iter;

  // look in database to find attributes of the table we want to write
  getOIDs(tableName,relationNames,table_oids);

  for(size_t i = 0; i < cols.size(); i++) {
    iter = find(relationNames.begin(), relationNames.end(),cols[i].colname);
    if(iter == relationNames.end()) {
      throw unknownRelationName(cols[i].colname);
    } else {
      offset = distance(relationNames.begin(),iter);
      oids.push_back(table_oids[offset]);
    }
  }
}

int PostgresConnection::write(const char* tableName, const vector<ColumnForWriting>& cols, const int nrows) {
  const char* prepared_stmt_name = "myinsert";
  int currentRow = 0;
  PGresult* res = NULL;
  PGresult* deallocate_res = NULL;
  vector<PostgresColumnWriter*> pg_col_writers;
  vector<size_t> validColumns;
  vector<string> validColnames;
  vector<Oid> oids;

  try {
    find_column_oids(oids, tableName, cols);
  } catch(unknownRelationName& e) {
    REprintf(e.what());
    return 0;
  }

  // allocate everything based on the size of validColumns
  int numCols = cols.size();
  Oid* paramTypes = new Oid[numCols];
  char** paramValues = new char*[numCols];
  int* paramLengths = new int[numCols];
  int* paramFormats = new int[numCols];
  int resultFormat = 1;

  std::string command(genInsertQuery(tableName, cols));

  for(int i = 0; i < numCols; i++) {
    paramTypes[i] = oids[i];
    // a colwrite object is initialized with the memory locations it will be writing to
    pg_col_writers.push_back(PostgresColumnWriter::createPostgresColumnWriter(paramTypes[i],cols[i],paramValues[i],paramLengths[i]));
  }

  // init paramFormats
  for(int i = 0; i < numCols; i++) {
    paramFormats[i] = pg_col_writers[i]->getFormat();
  }

  res = PQprepare(conn_,
		  prepared_stmt_name,
		  command.c_str(),
		  numCols,
		  paramTypes);
  if(PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    return 0;
  }
  PQclear(res);
  res = NULL;

  transaction_begin();
  do {
    PQclear(res);
    // update pointer set
    for(int i = 0; i < numCols; i++) {
      pg_col_writers[i]->update(currentRow);
    }

    // insert
    res = PQexecPrepared(conn_,
			 prepared_stmt_name,
			 numCols,
			 paramValues,
			 paramLengths,
			 paramFormats,
			 resultFormat);
    ++currentRow;
  } while(currentRow < nrows && PQresultStatus(res) == PGRES_COMMAND_OK);

  if(PQresultStatus(res) == PGRES_COMMAND_OK) {
    commit();
  } else {
    REprintf("%s\n",PQresStatus(PQresultStatus(res)));
    REprintf("%s\n",PQresultErrorMessage(res));
    rollback();
  }

  delete[] paramTypes;
  delete[] paramValues;
  delete[] paramLengths;
  delete[] paramFormats;

  for(int i = 0; i < numCols; i++) {
    delete pg_col_writers[i];
  }
  deallocate_res = PQexec(conn_,(string("DEALLOCATE ") + string(prepared_stmt_name)).c_str());
  if(PQresultStatus(deallocate_res) != PGRES_COMMAND_OK) {
    REprintf("problem w/ DEALLOCATE on prepared statement.");
  }
  PQclear(deallocate_res);

  if(PQresultStatus(res) != PGRES_COMMAND_OK) {
    REprintf(PQresStatus(PQresultStatus(res)));
    PQclear(res);
    // throw...
    return 0;
  }
  return currentRow;
}
