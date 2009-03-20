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
#include "postgres.connection.hpp"
#include "conversion.utils.hpp"
#include "utils.hpp"
#include "r.objects.hpp"

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
    throw DatabaseConnection::DriverNotSupported(dbType_s.c_str());
  }
}

void DatabaseConnection::createTable(const char* tableName, vector<string>& colnames, vector<TypeConverter*>& typeConverters) {
  stringstream query;

  if(typeConverters.size() != colnames.size()) {
    cerr << "colnames not the right size" << endl;
    return;
  }

  cleanColnames(colnames, string("."),"_");
  cleanColnames(colnames, string("("),"");
  cleanColnames(colnames, string(")"),"");
  cleanColnames(colnames, string(" "),"_");
  query << "CREATE TABLE " << tableName << endl << "(";
  for(size_t i = 0; i < colnames.size(); i++) {
    query << colnames[i] <<  " " << typeConverters[i]->getNativeType();
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
  Robject* my_sexp = Robject::factory(value_sexp);
  return my_sexp->writeToDatabase(this, tableName, writeRowNames);
}

bool DatabaseConnection::removeTable(const char* tableName) {
  stringstream query;
  query <<  "drop table " << tableName;
  QueryResults* res = sendQuery(query.str().c_str());
  if(res->valid()) {
    // FIXME: send total rows affected
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

void DatabaseConnection::cleanColnames(vector<string>& colnames, const string& badString, const string& replaceString) const {
  for(vector<string>::iterator iter = colnames.begin(); iter != colnames.end(); iter++) {
    cleanString(*iter, badString, replaceString);
  }
}
