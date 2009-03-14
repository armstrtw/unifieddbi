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

#ifndef POSTGRES_RESULTS_HPP
#define POSTGRES_RESULTS_HPP

#include <iostream>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "query.results.hpp"
#include "postgres.results.column.hpp"

using std::cout;
using std::endl;

class PostgresResults : public QueryResults {
private:
  const PGresult *res_;
public:
  PostgresResults(const PGresult *res);
  PostgresResults(PGresult *res);
  ~PostgresResults();
  void getStatus() const;
  bool valid() const;
  int nrow() const;
  int ncol() const;
  void getColnames(std::vector<std::string>& ans) const;
};

#endif // POSTGRES_RESULTS_HPP
