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
#include <boost/date_time/gregorian/gregorian.hpp>

#include "postgres.column.writer.hpp"
#include "postgres.oid.hpp"
#include "r.column.types.hpp"
#include "database.errors.hpp"
#include "conversion.utils.hpp"

using std::cout;
using std::endl;
using std::string;

using namespace boost::gregorian;
using namespace postgres;

PostgresColumnWriter* PostgresColumnWriter::createPostgresColumnWriter(const Oid oid, const ColumnForWriting& wjob, char*& dest, int& paramLength) {
  switch(static_cast<pg_oidT>(oid)) {
  case BOOLOID:
    switch(getColumnType(wjob.sexp)) {
    case boolT:
      return new bool2bool_writer(wjob, dest, paramLength);
    case intT:
      return new int2bool_writer(wjob, dest, paramLength);
    case charT:
      return new char2bool_writer(wjob, dest, paramLength);
    default:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("BOOLOID"));
    }
    break;
  case BYTEAOID:
    throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				  string("BYTEAOID"));
    break;
  //case INT8OID:
    //return new int8_writer(wjob, dest, paramLength);
  // case INT2OID:
  // case INT2VECTOROID:
  case INT4OID:
    switch(getColumnType(wjob.sexp)) {
    case boolT:
      return new bool2int4_writer(wjob, dest, paramLength);
    case intT:
      return new int2int4_writer(wjob, dest, paramLength);
    default:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("INT4OID"));
    }
    break;
  // case REGPROCOID:
  case CHAROID:
  case NAMEOID:
  case TEXTOID:
  case VARCHAROID:
  case BPCHAROID:
    switch(getColumnType(wjob.sexp)) {
    case charT:
      return new char2char_writer(wjob, dest, paramLength);
    case factorT:
      return new factor2char_writer(wjob, dest, paramLength);
    case dateTimeT:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("TEXTOID/VARCHAROID/BPCHAROID"));
    case dateT:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("TEXTOID/VARCHAROID/BPCHAROID"));
    default:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("TEXTOID/VARCHAROID/BPCHAROID"));
    }
    break;

  // case OIDOID:
  // case TIDOID:
  // case XIDOID:
  // case CIDOID:
  // case OIDVECTOROID:
  // case PG_TYPE_RELTYPE_OID:
  // case PG_ATTRIBUTE_RELTYPE_OID:
  // case PG_PROC_RELTYPE_OID:
  // case PG_CLASS_RELTYPE_OID:
  // case XMLOID:
  // case POINTOID:
  // case LSEGOID:
  // case PATHOID:
  // case BOXOID:
  // case POLYGONOID:
  // case LINEOID:
  // case FLOAT4OID:
  case FLOAT8OID:
    switch(getColumnType(wjob.sexp)) {
    case doubleT:
      return new double2float8_writer(wjob, dest, paramLength);
    default:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("FLOAT8OID"));
    }
    break;
  // case ABSTIMEOID:
  // case RELTIMEOID:
  // case TINTERVALOID:
  // case UNKNOWNOID:
  // case CIRCLEOID:
  // case CASHOID:
  // case MACADDROID:
  // case INETOID:
  // case CIDROID:
  // case INT4ARRAYOID:
  // case FLOAT4ARRAYOID:
  // case ACLITEMOID:
  // case CSTRINGARRAYOID:
  case DATEOID:
    switch(getColumnType(wjob.sexp)) {
    case dateT:
      return new posixct2date_writer(wjob, dest, paramLength);
    default:
      throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				    string("DATEOID"));
    }
    break;
  // case TIMEOID:
  // case TIMESTAMPOID:
  //case TIMESTAMPTZOID:
    //return new datetime_writer(wjob, dest, paramLength);
  // case INTERVALOID:
  // case TIMETZOID:
  // case BITOID:
  // case VARBITOID:
  // case NUMERICOID:
  // case REFCURSOROID:
  // case REGPROCEDUREOID:
  // case REGOPEROID:
  // case REGOPERATOROID:
  // case REGCLASSOID:
  // case REGTYPEOID:
  // case REGTYPEARRAYOID:
  // case TSVECTOROID:
  // case GTSVECTOROID:
  // case TSQUERYOID:
  // case REGCONFIGOID:
  // case REGDICTIONARYOID:
  // case RECORDOID:
  // case CSTRINGOID:
  // case ANYOID:
  // case ANYARRAYOID:
  // case VOIDOID:
  // case TRIGGEROID:
  // case LANGUAGE_HANDLEROID:
  // case INTERNALOID:
  // case OPAQUEOID:
  // case ANYELEMENTOID:
  // case ANYNONARRAYOID:
  // case ANYENUMOID:
  default:
    cout << "oid: " << oid << endl;
    throw MapToTypeNotImplemented(columnType2String(getColumnType(wjob.sexp)),
				  string("OID Not Implemented"));
  }
}

void posixct2date_writer::setCharPtr(const R_len_t row) {
  const date pg_origin = date(2000,Jan,1);
  time_t x_time_t = static_cast<time_t>(REAL(wjob_.sexp)[row + wjob_.offset]);
  struct tm x_struct_tm;
  memset(&x_struct_tm,'\0',sizeof(struct tm));
  x_struct_tm.tm_isdst = -1;
  localtime_r(&x_time_t, &x_struct_tm);
  date greg_date = date_from_tm(x_struct_tm);
  date_duration pg_julian2000(greg_date - pg_origin);
  int julian_for_pg = static_cast<int>(pg_julian2000.days());
  hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&julian_for_pg));
  dest_ = reinterpret_cast<char*>(&hton_flipped_int);
}
