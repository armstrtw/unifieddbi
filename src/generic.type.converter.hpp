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

#ifndef GENERIC_TYPE_CONVERTER_HPP
#define GENERIC_TYPE_CONVERTER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <Rinternals.h>

#include "type.converter.hpp"
#include "utils.hpp"

class GenericTypeConverter_default : public TypeConverter {
 public:
  GenericTypeConverter_default(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    return std::string("");
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    return std::string("");
  }
};


class GenericTypeConverter_integer : public TypeConverter {
 public:
  GenericTypeConverter_integer(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << INTEGER(sexp_)[row];
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    std::stringstream ans;
    ans << INTEGER(sexp_)[col*row +row];
    return ans.str();
  }
};


class GenericTypeConverter_double : public TypeConverter {
 public:
  GenericTypeConverter_double(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans.precision(52); //IEEE 754
    ans << REAL(sexp_)[row];
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    std::stringstream ans;
    ans << REAL(sexp_)[col*row +row];
    return ans.str();
  }
};

class GenericTypeConverter_char : public TypeConverter {
 public:
  GenericTypeConverter_char(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << "'";
    ans << std::string(CHAR(STRING_ELT(sexp_,row)));
    ans << "'";
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    std::stringstream ans;
    ans << "'";
    ans << std::string(CHAR(STRING_ELT(sexp_,col*row +row)));
    ans << "'";
    return ans.str();
  }
};

class GenericTypeConverter_boolean : public TypeConverter {
public:
  GenericTypeConverter_boolean(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << "'";
    ans << LOGICAL(sexp_)[row];
    ans << "'";
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    std::stringstream ans;
    ans << LOGICAL(sexp_)[col*row +row];
    return ans.str();
  }
};


class GenericTypeConverter_dateFromPOSIXct : public TypeConverter {
public:
  GenericTypeConverter_dateFromPOSIXct(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << "'";
    ans << posixDate2string(REAL(sexp_)[row]);
    ans << "'";
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    return posixDate2string(REAL(sexp_)[col*row +row]);
  }
};

class GenericTypeConverter_dateFromPOSIXlt : public TypeConverter {
 public:
  GenericTypeConverter_dateFromPOSIXlt(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    // col 6 is year - 1900
    // col 5 mon - 1
    // col 4 is day
    return posixltDate2string(static_cast<int>(REAL(VECTOR_ELT(sexp_,6))[row] + 1900),
			  static_cast<int>(REAL(VECTOR_ELT(sexp_,5))[row] + 1),
			  static_cast<int>(REAL(VECTOR_ELT(sexp_,4))[row]));
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    // never called
    return std::string("");
  }
};

class GenericTypeConverter_charFromFactor : public TypeConverter {
 private:
  SEXP levels_;
  std::string factorLevelAtIndex(const R_len_t i) {
    return std::string(CHAR(STRING_ELT(levels_,INTEGER(sexp_)[i]-1)));
  }
 public:
  GenericTypeConverter_charFromFactor(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType), levels_(getAttrib(x,R_LevelsSymbol)) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << "'";
    ans << factorLevelAtIndex(row);
    ans << "'";
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    std::stringstream ans;
    ans << "'";
    ans << factorLevelAtIndex(col*row +row);
    ans << "'";
    return ans.str();
  }
};


// GenericTypeConverter_datetimeFromPOSIXct
class GenericTypeConverter_datetimeFromPOSIXct : public TypeConverter {
public:
  GenericTypeConverter_datetimeFromPOSIXct(SEXP x, const std::string& nativeType) : TypeConverter(x, nativeType) {}

  const std::string toString(const R_len_t row) {
    std::stringstream ans;
    ans << "'";
    ans << posixDateTime2string(REAL(sexp_)[row]);
    ans << "'";
    return ans.str();
  }

  const std::string toString(const R_len_t row, const R_len_t col) {
    return posixDateTime2string(REAL(sexp_)[col*row +row]);
  }
};

// GenericTypeConverter_datetimeFromPOSIXlt


#endif // GENERIC_TYPE_CONVERTER_HPP
