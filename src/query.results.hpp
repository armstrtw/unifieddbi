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

#ifndef QUERY_RESULTS_HPP
#define QUERY_RESULTS_HPP

#include <string>
#include <vector>
#include <Rinternals.h>

#include "query.results.column.hpp"
#include "column.factory.hpp"

class QueryResults {
private:
  ColumnFactory* columnFactory_;
  int completedRows_;
  std::vector<QueryResultColumn*> queryResultColumns_;
public:
  QueryResults(ColumnFactory* columnFactory);
  virtual ~QueryResults();
  virtual void printStatus() const = 0;
  virtual bool valid() const = 0;
  virtual int nrow() const = 0;

  // implemented in base class
  const int ncol() const;
  void getColnames(std::vector<std::string>& ans) const;
  SEXP allocSEXP(const R_len_t fetch_rows) const;
  SEXP fetch(const int nrows);

};


#endif // QUERY_RESULTS_HPP
