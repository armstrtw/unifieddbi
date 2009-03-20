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
#include "generic.type.converter.hpp"
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
  PQfinish(conn_);
  conn_ = NULL;
}

PostgresConnection::PostgresConnection(): conn_(static_cast<PGconn*>(NULL)) {}

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

bool PostgresConnection::existsTable(const char* tableName) {
  stringstream query;
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

PostgresResults* PostgresConnection::sendQuery(const char* query) {
  if(query && connectionValid()) {
    //return new PostgresResults(static_cast<const PGresult*>(PQexec(conn_,query)));
    return new PostgresResults(static_cast<const PGresult*>(PQexecParams(conn_,query,0,NULL,NULL,NULL,NULL,1)));
  }
  cerr << "bad connection to database" << endl;
  return static_cast<PostgresResults*>(NULL);
}


// this function needs the whole object to determine type
// for these reasons
// 1) POSIXct -- uses the same R class to represent both date and timestamp
//    the only way to determine which database type to use is to scan the dates
//    to see if there are any with precision greater than %Y-%m-%d
// 2) factors, could be uploaded as enums, for now, just use characters
// 3) character types, should they really be uploaded as text as the RPostgreSQL
//    package does?  I think varchar could be used until we hit a a certain character limit
//    this is somewhat heuristic, but I think it's better than using text for all character objects
TypeConverter* PostgresConnection::getTypeConverter(SEXP value_sexp) {
  vector<string> object_class;
  sexp2string(getAttrib(value_sexp,R_ClassSymbol), back_inserter(object_class));

  if(object_class.empty()) {
    switch(TYPEOF(value_sexp)) {
    case LGLSXP:
      return new GenericTypeConverter_boolean(value_sexp, "boolean");
    case INTSXP:
      return new GenericTypeConverter_integer(value_sexp, "integer");
    case REALSXP:
      return new GenericTypeConverter_double(value_sexp, "double precision");
    case STRSXP:
      return new GenericTypeConverter_char(value_sexp, "character varying");
    case EXTPTRSXP:
    case WEAKREFSXP:
    case RAWSXP:
      /* these will be bytea */
    case CPLXSXP: /* need a complex type in postgres before this can be supported */
    default:
      throw DatabaseConnection::TypeNotSupported("(no class attribute found)");
    }
  }

  if(object_class[0]=="integer") {
    return new GenericTypeConverter_integer(value_sexp,"integer");
  }

  if(object_class[0]=="numeric") {
    return new GenericTypeConverter_double(value_sexp,"double precision");
  }

  if(object_class[0]=="POSIXt") {
    if(object_class.size() > 1 && object_class[1]=="POSIXct") {
      if(posixHasTimes(value_sexp)) {
	//FIXME: return new GenericTypeConverter_datetimeFromPOSIXct(value_sexp);
	return new GenericTypeConverter_default(value_sexp,"integer");
      } else {
	return new GenericTypeConverter_dateFromPOSIXct(value_sexp,"date");
      }
    }
    if(object_class.size() > 1 && object_class[1]=="POSIXlt") {
      if(posixHasTimes(value_sexp)) {
	//FIXME: return new GenericTypeConverter_datetimeFromPOSIXlt(value_sexp,"timestamp with timezone");
	return new GenericTypeConverter_default(value_sexp,"integer");
      } else {
	return new GenericTypeConverter_dateFromPOSIXlt(value_sexp,"date");
      }
    }
  }

  if(object_class[0]=="factor") {
    return new GenericTypeConverter_charFromFactor(value_sexp,"character_varying");
  }

  if(object_class[0]=="character") {
    return new GenericTypeConverter_char(value_sexp,"character_varying");
  }

  if(object_class[0]=="logical") {
    return new GenericTypeConverter_boolean(value_sexp,"boolean");
  }
  throw DatabaseConnection::TypeNotSupported(object_class[0].c_str());
}
