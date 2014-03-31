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

#include <vector>
#include <string>
#include <iostream>

#include <Rutilities.hpp>

#include "r.column.types.hpp"
#include "utils.hpp"
#include "database.errors.hpp"

using std::vector;
using std::string;

RColumnType getColumnType(const SEXPTYPE sptype) {
  switch(sptype) {
  case LGLSXP: return boolT;
  case INTSXP: return intT;
  case REALSXP: return doubleT;
  case STRSXP: return charT;
  // case EXTPTRSXP: return notDefinedT;
  // case WEAKREFSXP: return notDefinedT;
  // case RAWSXP: return notDefinedT;
  // case CPLXSXP: return notDefinedT;
  default:
    throw TypeNotSupported("(no class attribute found)");
  }
}

// this function needs the whole object to determine type
// for these reasons
// 1) POSIXct -- uses the same R class to represent both date and timestamp
//    the only way to determine which database type to use is to scan the dates
//    to see if there are any with precision greater than %Y-%m-%d
// 2) one can't tell what the actual type of an object is by looking at TYPEOF
//    factors are stored as int's, date types can be either double or int
//    one simply can't tell what the actual object is just by using TYPEOF
RColumnType getColumnType(SEXP value_sexp) {
  vector<string> object_class;
  sexp2string(getAttrib(value_sexp,R_ClassSymbol), back_inserter(object_class));

  // remove asis class
  // since it's not the class we really care about
  // need to do this before anything else
  if(!object_class.empty() && object_class[0]=="AsIs") {
    object_class.erase(object_class.begin());
  }

  if(object_class.empty()) {
    return getColumnType(TYPEOF(value_sexp));
  }

  string most_specialized_class = *object_class.rbegin();
  string least_specialized_class = *object_class.begin();

  if(most_specialized_class=="integer") { return intT; }
  if(most_specialized_class=="numeric") { return doubleT; }
  if(most_specialized_class=="factor") { return factorT; }
  if(most_specialized_class=="character") { return charT; }
  if(most_specialized_class=="logical") { return boolT; }
  if(most_specialized_class=="POSIXct" || least_specialized_class=="POSIXct") { return dateTimeT; }
  if( (most_specialized_class=="Date" || least_specialized_class=="Date") && TYPEOF(value_sexp)==INTSXP) { return intDateT; }
  if( (most_specialized_class=="Date" || least_specialized_class=="Date") && TYPEOF(value_sexp)==REALSXP)  { return doubleDateT; }
  // not a common class, so just use basic type to convert
  return getColumnType(TYPEOF(value_sexp));
}


std::string columnType2String(const RColumnType col) {
  switch(col) {
  case boolT:
    return string("boolT");
  case intT:
    return string("intT");
  case doubleT:
    return string("doubleT");
  case charT:
    return string("charT");
  case factorT:
    return string("factorT");
  case dateTimeT:
    return string("dateTimeT");
  case intDateT:
    return string("intDateT");
  case doubleDateT:
    return string("doubleDateT");
  default:
    return string("not defined");
  }
}
