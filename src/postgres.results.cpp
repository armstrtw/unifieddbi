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

#include <vector>
#include <string>
#include <iostream>
#include <libpq-fe.h>
#include <Rinternals.h>

#include "postgres.results.hpp"
#include "postgres.results.column.hpp"
#include "postgres.column.factory.hpp"

using namespace postgres;

PostgresResults::PostgresResults(const PGresult *res) : QueryResults(new PostgresColumnFactory(res)), res_(res) {
  //cout << "PostgresResults::PostgresResults(const PGresult *res)" << endl;
}

PostgresResults::~PostgresResults() {
  //cout << "PostgresResults::~PostgresResults" << endl;
  PQclear(const_cast<PGresult*>(res_));
}

void PostgresResults::printStatus() const {
  ExecStatusType query_status = PQresultStatus(res_);
  REprintf("status: %s\n",PQresStatus(query_status));
  if(query_status != PGRES_COMMAND_OK) {
    REprintf("error msg: %s\n",PQresultErrorMessage(res_));
  }
}

bool PostgresResults::valid() const {
  //REprintf("PQcmdStatus: %s\n",PQcmdStatus(const_cast<PGresult*>(res_)));
  //printStatus();
  const int res_status = PQresultStatus(res_);
  if(res_status == PGRES_COMMAND_OK ||
     res_status == PGRES_TUPLES_OK) {
    return true;
  }
  return false;
}

int PostgresResults::nrow() const {
  return PQntuples(res_);
}

int PostgresResults::ncol() const {
  return PQnfields(res_);
}
