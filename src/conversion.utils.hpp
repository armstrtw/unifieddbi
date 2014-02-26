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

#ifndef CONVERSION_UTILS_HPP
#define CONVERSION_UTILS_HPP

#include <stdint.h>
#include <string>
#include <Rinternals.h>

union i32_float_union {
  uint32_t i32;
  float f;
};

union i64_double_union {
  uint64_t i64;
  double d;
};

union i32_int_union {
  uint32_t i32;
  int i;
};

std::string itos(const int i);
uint64_t ntohll(const uint64_t x);
void add_posixct_classes(SEXP x, const char* tzone);

#endif // CONVERSION_UTILS_HPP
