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

#ifndef R_COLUMN_TYPES_HPP
#define R_COLUMN_TYPES_HPP

#include <string>
#include <Rinternals.h>

enum RColumnType {boolT, intT, doubleT, charT, factorT, dateTimeT, intDateT, doubleDateT};

RColumnType getColumnType(const SEXPTYPE sptype);
RColumnType getColumnType(SEXP value_sexp);
std::string columnType2String(const RColumnType col);

#endif // R_COLUMN_TYPES_HPP
