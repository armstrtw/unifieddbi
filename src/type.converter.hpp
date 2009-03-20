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

#ifndef TYPE_CONVERTER_HPP
#define TYPE_CONVERTER_HPP

#include <string>

class TypeConverter {
protected:
  SEXP sexp_;
  const std::string nativeType_;
public:
  TypeConverter(SEXP x, const char* nativeType) : sexp_(x), nativeType_(nativeType) {}
  virtual ~TypeConverter() {}

  // vector form
  virtual const std::string toString(const R_len_t row) = 0;

  // matrix form
  virtual const std::string toString(const R_len_t row,const R_len_t col) = 0;

  std::string getNativeType() {
    return nativeType_;
  }
};

#endif // TYPE_CONVERTER_HPP
