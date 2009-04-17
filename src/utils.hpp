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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <Rinternals.h>

bool hasRownames(SEXP x);
SEXP getRownames(SEXP x);
SEXP getColnames(SEXP x);
bool posixHasTimes(SEXP x);
std::string posixDate2string(const double x);
std::string posixltDate2string(const int year, const int mon, const int day);
std::string posixDateTime2string(const double x);
//std::string posixltDateTime2string(const int year, const int mon, const int day);
void cleanString(std::string& str, const std::string& badString, const std::string& replaceString);
void cleanColnames(std::vector<std::string>& colnames, const std::string& badString, const std::string& replaceString);
std::string AddQuote(const std::string& x);

#endif // UTILS_HPP
