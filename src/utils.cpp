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
#include <vector>
#include <string>
#include <ctime>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <Rinternals.h>
#include <Rmath.h>
#include <Rutilities.hpp>

#include "utils.hpp"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace boost::gregorian;

// does not check to make sure it's a POSIXct object
bool posixHasTimes(SEXP x) {
  for(R_len_t i = 0; i < length(x); i++) {
    time_t x_time_t = static_cast<time_t>(REAL(x)[i]);
    struct tm x_struct_tm;
    memset(&x_struct_tm,'\0',sizeof(struct tm));
    x_struct_tm.tm_isdst = -1;
    localtime_r(&x_time_t, &x_struct_tm);
    if(x_struct_tm.tm_sec || x_struct_tm.tm_min || x_struct_tm.tm_hour) {
      return true;
    }
  }
  return false;
}

string posix2string(const double x) {
  const double secs_in_day = 60*60*24;
  boost::gregorian::date posix_epoch(1970,1,1);

  long days_since_epoch = static_cast<long>(fround(x/secs_in_day,0));
  boost::gregorian::date_duration dd(static_cast<long int>(days_since_epoch));
  boost::gregorian::date ans = posix_epoch + dd;
  return to_iso_extended_string(ans);
}

string posixlt2string(const int year, const int mon, const int day) {
  date ans(year, mon, day);
  return to_iso_extended_string(ans);
}
