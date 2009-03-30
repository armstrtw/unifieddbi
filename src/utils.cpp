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
#include <sstream>
#include <ctime>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Rinternals.h>
#include <Rmath.h>
#include <Rutilities.hpp>

#include "utils.hpp"

using std::vector;
using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using namespace boost::gregorian;
using namespace boost::posix_time;

// does not check to make sure it's a POSIXct object
bool posixHasTimes(SEXP x) {
  for(R_len_t i = 0; i < length(x); i++) {
    if(!ISNA(REAL(x)[i])) {
      time_t x_time_t = static_cast<time_t>(REAL(x)[i]);
      struct tm x_struct_tm;
      memset(&x_struct_tm,'\0',sizeof(struct tm));
      x_struct_tm.tm_isdst = -1;
      localtime_r(&x_time_t, &x_struct_tm);
      if(x_struct_tm.tm_sec || x_struct_tm.tm_min || x_struct_tm.tm_hour) {
        cout << "bad time: " << x_time_t << endl;
        return true;
      }
    }
  }
  return false;
}

string posixDateTime2string(const double x) {
  const char format[] = "%Y-%m-%d %H:%M:%S %Z";
  const int BUFFSIZE = 32;
  char ans_char[BUFFSIZE];
  struct tm posix_time_tm;

  const time_t posix_time_t = static_cast<time_t>(x);
  localtime_r(&posix_time_t,&posix_time_tm);
  strftime(ans_char,BUFFSIZE,format,&posix_time_tm);
  return string(ans_char);
}

string posixDate2string(const double x) {
  const char format[] = "%Y-%m-%d";
  const int BUFFSIZE = 16;
  char ans_char[BUFFSIZE];
  struct tm posix_time_tm;

  if(ISNA(x)) {
    return string();
  }

  const time_t posix_time_t = static_cast<time_t>(x);
  localtime_r(&posix_time_t,&posix_time_tm);
  strftime(ans_char,BUFFSIZE,format,&posix_time_tm);
  return string(ans_char);
}

string posixltDate2string(const int year, const int mon, const int day) {
  date ans(year, mon, day);
  return to_iso_extended_string(ans);
}


void cleanString(string& str, const string& badString, const string& replaceString) {
  string::size_type pos = 0;
  while ((pos = str.find(badString, pos)) != string::npos ) {
    str.replace( pos, badString.size(), replaceString );
    pos++;
  }
}

string AddQuote(const string& x) {
  stringstream ans;
  ans << "'";
  ans << x;
  ans << "'";

  return ans.str();
}
