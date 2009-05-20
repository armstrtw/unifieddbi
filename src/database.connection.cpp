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

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <cstddef>

#include <Rinternals.h>
#include <Rutilities.hpp>

#include "database.connection.hpp"
#include "database.errors.hpp"
#include "postgres.connection.hpp"
#include "conversion.utils.hpp"
#include "utils.hpp"
#include "r.objects.hpp"
#include "r.column.types.hpp"

using std::vector;
using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::transform;
using std::tolower;
using std::back_inserter;


DatabaseConnection* DatabaseConnection::init(const char* dbType) {
  if(dbType == NULL) {
    // throw
    return NULL;
  }

  string dbType_s(dbType);
  //transform(dbType_s.begin(), dbType_s.end(), dbType_s.begin(), tolower);
  if(dbType_s == "POSTGRES" || dbType_s == "PostgreSQL") {
    //if(dbType_s == "postgres") {
    return new postgres::PostgresConnection();
  } else {
    throw DriverNotSupported(dbType_s.c_str());
  }
}

void DatabaseConnection::fixColnames(vector<string>& colnames) {
  cleanColnames(colnames, string("."),"_");
  cleanColnames(colnames, string("("),"");
  cleanColnames(colnames, string(")"),"");
  cleanColnames(colnames, string(" "),"_");
}

void DatabaseConnection::createTable(const char* tableName, SEXP value_sexp, const bool writeRowNames) {

  std::vector<string> colnames;
  std::vector<SEXP> sexps;
  std::vector<RColumnType> colTypes;
  stringstream query;

  Robject* robj = Robject::factory(value_sexp);

  if(writeRowNames) {
    colnames.push_back("id");
    colTypes.push_back(charT);
  }

  robj->getColnames(colnames);
  robj->getSEXPS(sexps);
  delete robj;

  fixColnames(colnames);

  query << "CREATE TABLE " << tableName << endl << "(";
  for(size_t i = 0; i < colnames.size(); i++) {
    query << colnames[i] <<  " " << getNativeType(getColumnType(sexps[i]));
    if(i < (colnames.size() - 1)) {
      query << ",";
    }
    query << endl;
  }
  query << ")" << endl;
  // For debugging
  cout << query.str().c_str() << endl;
  QueryResults* res = sendQuery(query.str().c_str());
  res->printStatus();
}

int DatabaseConnection::writeTable(const char* tableName, SEXP value_sexp, const bool writeRowNames) {
  if(!existsTable(tableName)) {
    createTable(tableName, value_sexp, writeRowNames);
  }
  Robject* robj = Robject::factory(value_sexp);
  vector<ColumnForWriting> write_job;
  robj->createWriteJob(write_job, writeRowNames);
  return write(tableName, write_job, robj->nrow());
}

bool DatabaseConnection::removeTable(const char* tableName) {
  stringstream query;
  query <<  "drop table " << tableName;
  QueryResults* res = sendQuery(query.str().c_str());
  if(res->valid()) {
    return true;
  }
  return false;
}

SEXP DatabaseConnection::readTable(const char* tableName) {
  stringstream query;
  query << "select * from " << tableName << endl;
  QueryResults* res = sendQuery(query.str().c_str());
  return res->fetch(-1);
}

string DatabaseConnection::getNativeType(RColumnType dbColType) {
  switch(dbColType) {
  case boolT: return getBooleanType();
  case intT: return getIntegerType();
  case doubleT: return getDoubleType();
  case charT: return getCharacterType();
  case factorT: return getFactorType();
  case dateTimeT: return getDateTimeType();
  case dateT: return getDateType();
  }
}
