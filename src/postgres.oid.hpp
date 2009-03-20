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

#ifndef POSTGRES_OID_HPP
#define POSTGRES_OID_HPP

namespace postgres {
  enum pg_oidT {
    BOOLOID=16,
    BYTEAOID=17,
    CHAROID=18,
    NAMEOID=19,
    INT8OID=20,
    INT2OID=21,
    INT2VECTOROID=22,
    INT4OID=23,
    REGPROCOID=24,
    TEXTOID=25,
    OIDOID=26,
    TIDOID=27,
    XIDOID=28,
    CIDOID=29,
    OIDVECTOROID=30,
    PG_TYPE_RELTYPE_OID=71,
    PG_ATTRIBUTE_RELTYPE_OID=75,
    PG_PROC_RELTYPE_OID=81,
    PG_CLASS_RELTYPE_OID=83,
    XMLOID=142,
    POINTOID=600,
    LSEGOID=601,
    PATHOID=602,
    BOXOID=603,
    POLYGONOID=604,
    LINEOID=628,
    FLOAT4OID=700,
    FLOAT8OID=701,
    ABSTIMEOID=702,
    RELTIMEOID=703,
    TINTERVALOID=704,
    UNKNOWNOID=705,
    CIRCLEOID=718,
    CASHOID=790,
    MACADDROID=829,
    INETOID=869,
    CIDROID=650,
    INT4ARRAYOID=1007,
    FLOAT4ARRAYOID=1021,
    ACLITEMOID=1033,
    CSTRINGARRAYOID=1263,
    BPCHAROID=1042,
    VARCHAROID=1043,
    DATEOID=1082,
    TIMEOID=1083,
    TIMESTAMPOID=1114,
    TIMESTAMPTZOID=1184,
    INTERVALOID=1186,
    TIMETZOID=1266,
    BITOID=1560,
    VARBITOID=1562,
    NUMERICOID=1700,
    REFCURSOROID=1790,
    REGPROCEDUREOID=2202,
    REGOPEROID=2203,
    REGOPERATOROID=2204,
    REGCLASSOID=2205,
    REGTYPEOID=2206,
    REGTYPEARRAYOID=2211,
    TSVECTOROID=3614,
    GTSVECTOROID=3642,
    TSQUERYOID=3615,
    REGCONFIGOID=3734,
    REGDICTIONARYOID=3769,
    RECORDOID=2249,
    CSTRINGOID=2275,
    ANYOID=2276,
    ANYARRAYOID=2277,
    VOIDOID=2278,
    TRIGGEROID=2279,
    LANGUAGE_HANDLEROID=2280,
    INTERNALOID=2281,
    OPAQUEOID=2282,
    ANYELEMENTOID=2283,
    ANYNONARRAYOID=2776,
    ANYENUMOID=3500};

}  // namespace postgres
#endif // POSTGRES_OID_HPP
