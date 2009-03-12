#include <iostream>
#include "postgres.connection.hpp"
#include "query.results.hpp"

using std::cerr;
using std::endl;

static void connFinalizer(SEXP dbi_conn_sexp) {
  if(!R_ExternalPtrAddr(dbi_conn_sexp)) {
    return;
  }
  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));

  // call c++ destructor
  delete conn;

  R_ClearExternalPtr(dbi_conn_sexp);
}

static void queryResultsFinalizer(SEXP query_results_sexp) {
  if(!R_ExternalPtrAddr(query_results_sexp)) {
    return;
  }
  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(query_results_sexp));

  // call c++ destructor
  delete query_results;
  
  R_ClearExternalPtr(query_results_sexp);
}

SEXP dbConnect(SEXP dbType_sexp, SEXP connection_string_sexp) {
  SEXP dbi_conn_sexp;
  DatabaseConnection* conn = NULL;
  const char* dbType = CHAR(STRING_PTR(dbType_sexp)[0]);
  const char* connection_string = CHAR(STRING_PTR(dbType_sexp)[0]);

  // this test is to check whether the package was compiled with support
  // for this specific dbType
  try {
    conn = DatabaseConnection::init(dbType);
  } catch (DatabaseConnection::DriverNotSupported& e) {
    cerr << e.what() << endl;
    return R_NilValue;
  }
  
  // if we succeed then return a wrapped connection, otherwise return null
  try {
    conn->connect(connection_string);
  } catch(DatabaseConnection::BadDatabaseConnection& e) {
    cerr << e.what() << endl;
    return R_NilValue;
  }

  PROTECT(dbi_conn_sexp = R_MakeExternalPtr(reinterpret_cast<void*>(conn),install("DBI_conn_pointer"),R_NilValue));
  R_RegisterCFinalizerEx(dbi_conn_sexp, connFinalizer, TRUE); 
  return dbi_conn_sexp;
}

SEXP dbConnectWithParams(SEXP dbType_sexp, SEXP user_sexp, SEXP pass_sexp) {
  const char* user = CHAR(STRING_PTR(user_sexp)[0]);
  const char* pass = CHAR(STRING_PTR(pass_sexp)[0]);
  /* not implemented */
  cerr << "not implemented yet." << endl;
  return R_NilValue;
}

SEXP dbSendQuery(SEXP dbi_conn_sexp, SEXP qry_sexp) {
  SEXP dbi_query_results_sexp;

  const char* qry = CHAR(STRING_PTR(qry_sexp)[0]);
  DatabaseConnection* conn = reinterpret_cast<DatabaseConnection*>(R_ExternalPtrAddr(dbi_conn_sexp));
  QueryResults* query_results = conn->sendQuery(qry);
  
  PROTECT(dbi_query_results_sexp = R_MakeExternalPtr(reinterpret_cast<void*>(query_results),install("DBI_results_pointer"),R_NilValue));
  R_RegisterCFinalizerEx(dbi_query_results_sexp, queryResultsFinalizer, TRUE);
  UNPROTECT(1);
  return dbi_query_results_sexp;
}

SEXP dbfetch(SEXP dbi_query_results_sexp, SEXP nrows_sexp) {
  SEXP ans;
  const int nrows = INTEGER(nrows_sexp)[0];
  QueryResults* query_results = reinterpret_cast<QueryResults*>(R_ExternalPtrAddr(dbi_query_results_sexp));

  // FIXME: try/catch instead?
  if(query_results) {
    return query_results->fetch(nrows);
  } else {
    return R_NilValue;
  }
}
