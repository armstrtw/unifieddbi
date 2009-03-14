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

#ifndef QUERY_RESULTS_COLUMN_HPP
#define QUERY_RESULTS_COLUMN_HPP

#include <Rinternals.h>

class QueryResultColumn {
protected:
  QueryResultColumn(const int position): position_(position) {}
  const int position_;
public:
  virtual ~QueryResultColumn() {}
  virtual const char* getName() const = 0;
  virtual SEXP allocateSEXP(const R_len_t nrows) const = 0;
  virtual void setValue(SEXP x, const R_len_t row) const = 0;
};

#endif // QUERY_RESULTS_COLUMN_HPP
