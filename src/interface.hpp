#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <Rinternals.h>

static void connFinalizer(SEXP dbi_conn_sexp);
static void queryResultsFinalizer(SEXP query_results_sexp);

extern "C" {
  SEXP dbConnect(SEXP dbType_sexp, SEXP connection_string_sexp);
  SEXP dbConnectWithParams(SEXP dbType_sexp, SEXP user_sexp, SEXP pass_sexp);
  SEXP dbSendQuery(SEXP dbi_conn_sexp, SEXP qry_sexp);
  SEXP dbfetch(SEXP dbi_query_results_sexp, SEXP nrows_sexp);
}

#endif // INTERFACE_HPP
