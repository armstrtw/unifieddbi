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
#include <vector>
#include <sstream>
#include <string>
#include "r.objects.hpp"
#include "Rutilities.hpp"

using std::vector;
using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;

Robject* Robject::factory(SEXP x) {
  SEXP r_class = getAttrib(x,R_ClassSymbol);
  const std::string r_class_str(CHAR(STRING_ELT(r_class,0)));
  if(r_class == R_NilValue) {
    //throw()
    return NULL;
  }
  if(r_class_str == "matrix") {
    return new RMatrix(x);
  }

  if(r_class_str == "data.frame") {
    return new RDataFrame(x);
  }

  return new RVector(x);
}

bool RDataFrame::hasRownames() const {
  return (getRownames() == R_NilValue) ? false : true;
}

bool RDataFrame::hasColnames() const {
  return (getColnames() == R_NilValue) ? false : true;
}

SEXP RDataFrame::getRownames() const {
  return getAttrib(sexp_, install("row.names"));
}

SEXP RDataFrame::getColnames() const {
  return getAttrib(sexp_, R_NamesSymbol);
}

bool RVector::hasNames() const {
  return (getNames() == R_NilValue) ? false : true;
}

SEXP RVector::getNames() const {
  return getAttrib(sexp_, R_NamesSymbol);
}

int RMatrix::writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames) {
  cout << "RMatrix::writeToDatabase" << endl;
  cout << "not implemented yet." << endl;
  return 0;
}

int RVector::writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames) {
  cout << "RVector::writeToDatabase" << endl;
  cout << "not implemented yet." << endl;
  return 0;
}

int RDataFrame::writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames) {
  R_len_t rows_to_write = length(VECTOR_ELT(sexp_,0));
  int rows_written = 0;
  bool query_success = true;
  vector<TypeConverter*> typeConverters;
  vector<string> colnames;

  if(writeNames) {
    typeConverters.push_back(conn->getTypeConverter(getRownames()));
    colnames.push_back("id");  // rails style -- shold there be an option for this arg?
  }

  for(R_len_t i = 0; i < length(sexp_); i++) {
    typeConverters.push_back(conn->getTypeConverter(VECTOR_ELT(sexp_,i)));
  }
  if(hasColnames()) {
    sexp2string(getColnames(),back_inserter(colnames));
  } else {
    for(R_len_t i = 0; i < length(sexp_); i++) {
      stringstream fakename("colname");
      fakename << i + 1;
      colnames.push_back(fakename.str());
    }
  }

  if(!conn->existsTable(tableName)) {
    conn->createTable(tableName,colnames,typeConverters);
  }
  conn->transaction_begin();
  do {
    stringstream query;
    query << "insert into " << tableName << " values (";
    for(size_t col = 0; col < typeConverters.size(); col++) {
      query << typeConverters[col]->toString(rows_written);
      if(col < (typeConverters.size() - 1)) {
	query << ",";
      }
    }
    query << ")";
    cout << query.str() << endl;
    QueryResults* res = conn->sendQuery(query.str().c_str());
    query_success = res->valid();
    rows_written += static_cast<int>(query_success);
    delete res;
    if(rows_written % 100 == 0) {
      cout << rows_written << endl;
    }
  } while(rows_written < rows_to_write && query_success);
  conn->commit();

  // free typeconverters
  for(vector<TypeConverter*>::iterator iter = typeConverters.begin(); iter != typeConverters.end(); iter++) {
    delete *iter;
  }
  return rows_written;
}
