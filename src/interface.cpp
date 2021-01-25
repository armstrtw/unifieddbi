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

#include "interface.hpp"
#include "database.errors.hpp"
#include "postgres.connection.hpp"
#include "query.results.hpp"

static void connFinalizer(SEXP dbi_conn_sexp) {
  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(conn) {
    // call c++ destructor
    delete conn;
    R_ClearExternalPtr(dbi_conn_sexp);
  }
}

static void queryResultsFinalizer(SEXP query_results_sexp) {
  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(query_results_sexp));
  if(query_results) {
    // call c++ destructor
    delete query_results;
    R_ClearExternalPtr(query_results_sexp);
  }
}

SEXP dbClearResult(SEXP dbi_query_results_sexp) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = static_cast<int>(false);

  if(TYPEOF(dbi_query_results_sexp) != EXTPTRSXP || dbi_query_results_sexp == R_NilValue) {
    UNPROTECT(1);
    return ans;
  }
  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(dbi_query_results_sexp));

  if(query_results) {
    // call c++ destructor
    delete query_results;
    R_ClearExternalPtr(dbi_query_results_sexp);
    LOGICAL(ans)[0] = static_cast<int>(true);
  }

  UNPROTECT(1);
  return ans;
}

SEXP dbDisconnect(SEXP dbi_conn_sexp) {
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1)); LOGICAL(ans)[0] = static_cast<int>(false);

  if(TYPEOF(dbi_conn_sexp) != EXTPTRSXP || dbi_conn_sexp == R_NilValue) {
    UNPROTECT(1);
    return ans;
  }

  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(conn) {
    // call c++ destructor
    delete conn;
    R_ClearExternalPtr(dbi_conn_sexp);
    LOGICAL(ans)[0] = static_cast<int>(true);
  } else {
    LOGICAL(ans)[0] = static_cast<int>(false);
  }
  UNPROTECT(1);
  return ans;
}


SEXP dbConnect(SEXP dbType_sexp,
	       SEXP connection_string_sexp,
	       SEXP user_sexp,
	       SEXP pass_sexp,
	       SEXP host_sexp,
	       SEXP port_sexp,
	       SEXP tty_sexp,
	       SEXP dbName_sexp,
	       SEXP options_sexp) {

  SEXP dbi_conn_sexp;
  DatabaseConnection* conn = NULL;
  const char* dbType = CHAR(STRING_PTR(dbType_sexp)[0]);
  const char* connection_string = (connection_string_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(connection_string_sexp)[0]);
  const char* user = (user_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(user_sexp)[0]);
  const char* pass = (pass_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(pass_sexp)[0]);
  const char* host = (host_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(host_sexp)[0]);
  const char* port = (port_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(port_sexp)[0]);
  const char* tty = (tty_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(tty_sexp)[0]);
  const char* dbName = (dbName_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(dbName_sexp)[0]);
  const char* options = (options_sexp == R_NilValue) ? NULL : CHAR(STRING_PTR(options_sexp)[0]);

  // this test is to check whether the package was compiled with support
  // for this specific dbType
  try {
    conn = DatabaseConnection::init(dbType);
  } catch (DriverNotSupported& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }

  // if we succeed then return a wrapped connection, otherwise return null
  try {
    // if user provides connection_string, then use it, otherwise try traditional args
    if(connection_string) {
      conn->connect(connection_string);
    } else {
      conn->connect(user,pass,host,port,tty,dbName,options);
    }
  } catch(BadDatabaseConnection& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }

  PROTECT(dbi_conn_sexp = R_MakeExternalPtr(reinterpret_cast<void*>(conn),install("DBI_conn_pointer"),R_NilValue));
  R_RegisterCFinalizerEx(dbi_conn_sexp, connFinalizer, TRUE);
  UNPROTECT(1);
  return dbi_conn_sexp;
}

SEXP dbSendQuery(SEXP dbi_conn_sexp, SEXP qry_sexp) {
  if(TYPEOF(dbi_conn_sexp) != EXTPTRSXP || dbi_conn_sexp == R_NilValue) {
    return R_NilValue;
  }

  SEXP dbi_query_results_sexp;
  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(!conn) {
    // throw bad_connection_object
    return R_NilValue;
  }
  const char* qry = CHAR(STRING_PTR(qry_sexp)[0]);
  QueryResults* query_results = conn->sendQuery(qry);
  //query_results->getStatus();

  PROTECT(dbi_query_results_sexp = R_MakeExternalPtr(reinterpret_cast<void*>(query_results),install("DBI_results_pointer"),R_NilValue));
  R_RegisterCFinalizerEx(dbi_query_results_sexp, queryResultsFinalizer, TRUE);
  UNPROTECT(1);
  return dbi_query_results_sexp;
}

SEXP dbQueryValid(SEXP dbi_query_results_sexp) {

  if(TYPEOF(dbi_query_results_sexp) != EXTPTRSXP || dbi_query_results_sexp == R_NilValue) {
    return R_NilValue;
  }

  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(dbi_query_results_sexp));
  SEXP ans; PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = static_cast<int>(query_results->valid());
  UNPROTECT(1);
  return ans;
}

SEXP dbfetch(SEXP dbi_query_results_sexp, SEXP nrows_sexp) {
  if(TYPEOF(dbi_query_results_sexp) != EXTPTRSXP || dbi_query_results_sexp == R_NilValue) {
    return R_NilValue;
  }

  const int nrows = INTEGER(nrows_sexp)[0];
  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(dbi_query_results_sexp));

  if(query_results) {
    return query_results->fetch(nrows);
  } else {
    REprintf("invalide dbi_query_results object.\n");
    return R_NilValue;
  }
}

// return number of rows written to database
// using both overwrite and append is a bad design decision
// there should just be overwrite, which will drop the table if it exists
// append should be automatic if the table exists, and should fail if the row formats don't match up
// having both overwrite and append just complicates the logic of this function
SEXP dbWriteTable(SEXP dbi_conn_sexp, SEXP tableName_sexp, SEXP value_sexp, SEXP writeRowNames_sexp, SEXP overWrite_sexp, SEXP append_sexp) {
  SEXP ans;
  int rows;

 if(TYPEOF(dbi_conn_sexp) != EXTPTRSXP || dbi_conn_sexp == R_NilValue) {
    return R_NilValue;
  }

  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(!conn) {
    // throw bad_connection_object
    REprintf("bad database connection.\n");
    return ScalarInteger(0);
  }
  if(TYPEOF(tableName_sexp) != STRSXP) {
    REprintf("ERROR: tableName is not a string.\n");
    return ScalarInteger(0);
  }
  const char* tableName = CHAR(STRING_ELT(tableName_sexp,0));
  const bool writeRowNames = static_cast<bool>(LOGICAL(writeRowNames_sexp)[0]);
  const bool overWrite = static_cast<bool>(LOGICAL(overWrite_sexp)[0]);

  if(conn->existsTable(tableName) && overWrite) {
    if(!conn->removeTable(tableName)) {
      REprintf("could not remove existing table (aborting).\n");
      return ScalarInteger(0);
    }
  }

  try {
    rows = conn->writeTable(tableName, value_sexp, writeRowNames);
  } catch (MapToTypeNotImplemented& e) {
    REprintf("%s\n",e.what());
    return R_NilValue;
  }
  PROTECT(ans = allocVector(INTSXP,1));
  INTEGER(ans)[0] = rows;
  UNPROTECT(1);
  return ans;
}

SEXP dbExistsTable(SEXP dbi_conn_sexp, SEXP tableName_sexp) {
  SEXP ans;

 if(TYPEOF(dbi_conn_sexp) != EXTPTRSXP || dbi_conn_sexp == R_NilValue) {
    return R_NilValue;
  }

  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(!conn) {
    // throw bad_connection_object
    REprintf("bad database connection.\n");
    return R_NilValue;
  }

  const char* tableName = CHAR(STRING_ELT(tableName_sexp,0));
  PROTECT(ans = allocVector(LGLSXP,1));
  LOGICAL(ans)[0] = static_cast<int>(conn->existsTable(tableName));
  UNPROTECT(1);
  return ans;
}

SEXP dbListTables(SEXP dbi_conn_sexp) {

 if(TYPEOF(dbi_conn_sexp) != EXTPTRSXP || dbi_conn_sexp == R_NilValue) {
    return R_NilValue;
  }

  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  if(!conn) {
    // throw bad_connection_object
    REprintf("bad database connection.\n");
    return R_NilValue;
  }

  return conn->listTables();
}
