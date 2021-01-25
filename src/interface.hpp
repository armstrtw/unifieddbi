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

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <Rinternals.h>

static void connFinalizer(SEXP dbi_conn_sexp);
static void queryResultsFinalizer(SEXP query_results_sexp);

extern "C" {
  SEXP dbDisconnect(SEXP dbi_conn_sexp);
  SEXP dbConnect(SEXP dbType_sexp,
		 SEXP connection_string_sexp,
		 SEXP user_sexp,
		 SEXP pass_sexp,
		 SEXP host_sexp,
		 SEXP port_sexp,
		 SEXP tty_sexp,
		 SEXP dbName_sexp,
		 SEXP options_sexp);

  SEXP dbConnectWithParams(SEXP dbType_sexp, SEXP user_sexp, SEXP pass_sexp);
  SEXP dbSendQuery(SEXP dbi_conn_sexp, SEXP qry_sexp);
  SEXP dbQueryValid(SEXP dbi_query_results_sexp);
  SEXP dbfetch(SEXP dbi_query_results_sexp, SEXP nrows_sexp);
  SEXP dbClearResult(SEXP dbi_query_results_sexp);
  SEXP dbWriteTable(SEXP dbi_conn_sexp, SEXP tableName_sexp, SEXP value_sexp, SEXP writeRowNames_sexp, SEXP overWrite_sexp, SEXP append_sexp);
  SEXP dbExistsTable(SEXP dbi_conn_sexp, SEXP tableName_sexp);
  SEXP dbListTables(SEXP dbi_conn_sexp);
}

#endif // INTERFACE_HPP
