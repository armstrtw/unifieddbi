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
#include <cstring>
#include <netinet/in.h>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "postgres.column.writer.hpp"
#include "postgres.oid.hpp"
#include "r.column.types.hpp"
#include "database.errors.hpp"
#include "conversion.utils.hpp"

using std::cout;
using std::endl;

using namespace boost::gregorian;
using namespace postgres;

PostgresColumnWriter* PostgresColumnWriter::createPostgresColumnWriter(const Oid oid, const ColumnForWriting& wjob, char*& dest, int& paramLength) {
  switch(static_cast<pg_oidT>(oid)) {
  case BOOLOID:
    return new bool2bool_writer(wjob, dest, paramLength);
  case BYTEAOID:
    throw MapToTypeNotImplemented("BYTEAOID");
  case CHAROID:
  case NAMEOID:
    return new char2char_writer(wjob, dest, paramLength);
  //case INT8OID:
    //return new int8_writer(wjob, dest, paramLength);
  // case INT2OID:
  // case INT2VECTOROID:
  case INT4OID:
    switch(getColumnType(wjob.sexp)) {
    case boolT:
      throw MapToTypeNotImplemented("INT4OID");
    case intT:
      return new int2int4_writer(wjob, dest, paramLength);
    case doubleT:
      throw MapToTypeNotImplemented("INT4OID");
    case charT:
      throw MapToTypeNotImplemented("INT4OID");
    case factorT:
      throw MapToTypeNotImplemented("INT4OID");
    case dateTimeT:
      throw MapToTypeNotImplemented("INT4OID");
    case dateT:
      throw MapToTypeNotImplemented("INT4OID");
    default:
      throw MapToTypeNotImplemented("INT4OID");
    }
    break;
  // case REGPROCOID:
  case TEXTOID:
  case VARCHAROID:
  case BPCHAROID:
    switch(getColumnType(wjob.sexp)) {
    case boolT:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
    case intT:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
    case doubleT:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
    case charT:
      return new char2char_writer(wjob, dest, paramLength);
    case factorT:
      return new factor2char_writer(wjob, dest, paramLength);
    case dateTimeT:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
    case dateT:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
    default:
      throw MapToTypeNotImplemented("TEXTOID/VARCHAROID/BPCHAROID");
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
    case boolT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    case intT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    case doubleT:
      return new double2float8_writer(wjob, dest, paramLength);
    case charT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    case factorT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    case dateTimeT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    case dateT:
      throw MapToTypeNotImplemented("FLOAT8OID");
    default:
      throw MapToTypeNotImplemented("FLOAT8OID");
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
    case boolT:
      throw MapToTypeNotImplemented("DATEOID");
    case intT:
      throw MapToTypeNotImplemented("DATEOID");
    case doubleT:
      throw MapToTypeNotImplemented("DATEOID");
    case charT:
      throw MapToTypeNotImplemented("DATEOID");
    case factorT:
      throw MapToTypeNotImplemented("DATEOID");
    case dateTimeT:
      throw MapToTypeNotImplemented("DATEOID");
    case dateT:
      return new posixct2date_writer(wjob, dest, paramLength);
    default:
      throw MapToTypeNotImplemented("DATEOID");
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
    throw MapToTypeNotImplemented("OID Not Implemented");
  }
}

PostgresColumnWriter::PostgresColumnWriter(const ColumnForWriting& wjob, char*& dest, int& len) :
  wjob_(wjob), dest_(dest), len_(len) {
}

void PostgresColumnWriter::update(const R_len_t row) {
  setCharPtr(row);
  setLength(row);
}

bool2bool_writer::bool2bool_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
  len_ = sizeof(int);
}

void bool2bool_writer::setCharPtr(const R_len_t row) {
  dest_ = reinterpret_cast<char*>(&LOGICAL(wjob_.sexp)[row + wjob_.offset]);
}

// noop for bools
void bool2bool_writer::setLength(const R_len_t row) {}

int bool2bool_writer::getFormat() const {
  return 1;
}

char2char_writer::char2char_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
  len_ = NULL;
}

void char2char_writer::setCharPtr(const R_len_t row) {
  dest_ = const_cast<char*>(CHAR(STRING_ELT(wjob_.sexp, row + wjob_.offset)));
}

// noop for chars
void char2char_writer::setLength(const R_len_t row) {}

int char2char_writer::getFormat() const {
  return 0;
}

factor2char_writer::factor2char_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len), levels_(getAttrib(wjob_.sexp,R_LevelsSymbol)) {
  len_ = NULL;
}

void factor2char_writer::setCharPtr(const R_len_t row) {
  dest_ = const_cast<char*>(CHAR(STRING_ELT(levels_,INTEGER(wjob_.sexp)[row]-1)));
}

// noop for chars
void factor2char_writer::setLength(const R_len_t row) {}

int factor2char_writer::getFormat() const {
  return 0;
}

double2float8_writer::double2float8_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
  len_ = 8;
}

void double2float8_writer::setCharPtr(const R_len_t row) {
  hton_flipped_double = ntohll(*reinterpret_cast<uint64_t*>(&REAL(wjob_.sexp)[row + wjob_.offset]));
  dest_ = reinterpret_cast<char*>(&hton_flipped_double);
}

// noop for float8
void double2float8_writer::setLength(const R_len_t row) {}

int double2float8_writer::getFormat() const {
  return 1;
}

int2int4_writer::int2int4_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
  len_ = 4;
}

void int2int4_writer::setCharPtr(const R_len_t row) {
  hton_flipped_int = ntohl(*reinterpret_cast<uint32_t*>(&INTEGER(wjob_.sexp)[row + wjob_.offset]));
  dest_ = reinterpret_cast<char*>(&hton_flipped_int);
}

// noop for float8
void int2int4_writer::setLength(const R_len_t row) {}

int int2int4_writer::getFormat() const {
  return 1;
}

posixct2date_writer::posixct2date_writer(const ColumnForWriting& wjob, char*& dest, int& len) : PostgresColumnWriter(wjob, dest, len) {
  len_ = 4;
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

// noop for float8
void posixct2date_writer::setLength(const R_len_t row) {}

int posixct2date_writer::getFormat() const {
  return 1;
}
