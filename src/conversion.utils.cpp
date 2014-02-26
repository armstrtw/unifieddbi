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

#include <sstream>
#include <netinet/in.h>
#include "conversion.utils.hpp"

using std::string;
using std::stringstream;

string itos(const int i) {
  stringstream s;
  s << i;
  return s.str();
}

uint64_t ntohll(const uint64_t x) {

  typedef union
  {
    uint64_t value;
    uint32_t word[2];
  } ieee_type;

  ieee_type swap;
  swap.value = x;
  swap.word[0] = ntohl(swap.word[0]);
  swap.word[1] = ntohl(swap.word[1]);

  ieee_type ans;
  ans.word[0] = swap.word[1];
  ans.word[1] = swap.word[0];
  return ans.value;
}

void add_posixct_classes(SEXP x, const char* tzone) {
  // create and add dates class to dates object
  SEXP r_dates_class;
  PROTECT(r_dates_class = allocVector(STRSXP, 2));
  // the order of POSIXt, POSIXct frequently changes in R
  // as of 3.0.2, it is POSIXct,POSIXt
  SET_STRING_ELT(r_dates_class, 0, mkChar("POSIXct"));
  SET_STRING_ELT(r_dates_class, 1, mkChar("POSIXt"));
  classgets(x, r_dates_class);
  setAttrib(x, install("tzone"), mkString(tzone));
  UNPROTECT(1); //r_dates_class
}
