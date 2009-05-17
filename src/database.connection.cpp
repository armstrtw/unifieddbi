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
#include "generic.type.converter.hpp"
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
    throw DriverNotSupported(dbType_s.c_str());
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

// this function needs the whole object to determine type
// for these reasons
// 1) POSIXct -- uses the same R class to represent both date and timestamp
//    the only way to determine which database type to use is to scan the dates
//    to see if there are any with precision greater than %Y-%m-%d
TypeConverter* DatabaseConnection::getTypeConverter(SEXP value_sexp) {
  vector<string> object_class;
  sexp2string(getAttrib(value_sexp,R_ClassSymbol), back_inserter(object_class));

  // remove asis class
  // since it's not the class we really care about
  // need to do this before anything else
  if(!object_class.empty() && object_class[0]=="AsIs") {
    object_class.erase(object_class.begin());
  }

  if(object_class.empty()) {
    return getTypeConverterFromBasicType(value_sexp);
  }

  if(object_class[0]=="integer") {
    return new GenericTypeConverter_integer(value_sexp,getIntegerType());
  }

  if(object_class[0]=="numeric") {
    return new GenericTypeConverter_double(value_sexp,getDoubleType());
  }

  if(object_class[0]=="POSIXt") {
    if(object_class.size() > 1 && object_class[1]=="POSIXct") {
      if(posixHasTimes(value_sexp)) {
	return new GenericTypeConverter_datetimeFromPOSIXct(value_sexp,getDateTimeType());
      } else {
        cout << "returning GenericTypeConverter_dateFromPOSIXct for POSIXct" << endl;
	return new GenericTypeConverter_dateFromPOSIXct(value_sexp,getDateType());
      }
    }
    if(object_class.size() > 1 && object_class[1]=="POSIXlt") {
      if(posixHasTimes(value_sexp)) {
	//FIXME: return new GenericTypeConverter_datetimeFromPOSIXlt(value_sexp,"timestamp with timezone");
	return new GenericTypeConverter_default(value_sexp,"integer");
      } else {
	return new GenericTypeConverter_dateFromPOSIXlt(value_sexp,getDateType());
      }
    }
  }

  // "POSIXct" in 1st position (this can happen when people add the class manually)
  if(object_class[0]=="POSIXct") {
    if(posixHasTimes(value_sexp)) {
      return new GenericTypeConverter_datetimeFromPOSIXct(value_sexp,getDateTimeType());
    } else {
      return new GenericTypeConverter_dateFromPOSIXct(value_sexp,getDateType());
    }
  }

  // "POSIXlt" in 1st position (this can happen when people add the class manually)
  if(object_class[0]=="POSIXlt") {
    if(posixHasTimes(value_sexp)) {
      //FIXME: return new GenericTypeConverter_datetimeFromPOSIXct(value_sexp);
      return new GenericTypeConverter_default(value_sexp,"integer");
    } else {
      return new GenericTypeConverter_dateFromPOSIXlt(value_sexp,getDateType());
    }
  }

  if(object_class[0]=="factor") {
    return new GenericTypeConverter_charFromFactor(value_sexp,getCharacterType());
  }

  if(object_class[0]=="character") {
    return new GenericTypeConverter_char(value_sexp,getCharacterType());
  }

  if(object_class[0]=="logical") {
    return new GenericTypeConverter_boolean(value_sexp,getBooleanType());
  }

  // not a common class, so just use basic type to convert
  return getTypeConverterFromBasicType(value_sexp);
}

TypeConverter* DatabaseConnection::getTypeConverterFromBasicType(SEXP value_sexp) {
  switch(TYPEOF(value_sexp)) {
    case LGLSXP:
      return new GenericTypeConverter_boolean(value_sexp, getBooleanType());
    case INTSXP:
      return new GenericTypeConverter_integer(value_sexp, getIntegerType());
    case REALSXP:
      return new GenericTypeConverter_double(value_sexp, getDoubleType());
    case STRSXP:
      return new GenericTypeConverter_char(value_sexp, getCharacterType());
    case EXTPTRSXP:
    case WEAKREFSXP:
    case RAWSXP:
      /* these will be bytea */
    case CPLXSXP: /* need a complex type in postgres before this can be supported */
    default:
      throw TypeNotSupported("(no class attribute found)");
    }
}
