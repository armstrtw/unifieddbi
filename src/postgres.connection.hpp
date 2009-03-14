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

#ifndef POSTGRES_CONNECTION_HPP
#define POSTGRES_CONNECTION_HPP

#include <libpq-fe.h>
#include <Rinternals.h>

#include "database.connection.hpp"
#include "postgres.results.hpp"

namespace postgres {

  class PostgresConnection : public DatabaseConnection {
  private:
    PGconn* conn_;
    PostgresConnection();
    ~PostgresConnection();
    friend class DatabaseConnection;
    void verifyConnection();
    bool connectionValid();
  public:
    void connect(const char* connectionString);
    void disConnect();
    bool commit();
    bool rollback();
    SEXP listTables();
    SEXP readTable(const char* tableName);
    int writeTable(const char* tableName, SEXP x, const bool writeRowNames, const bool overwrite, const bool append);
    bool existsTable(const char* tableName);
    bool removeTable(const char* tableName);
    PostgresResults* sendQuery(const char* query);
  };
} // namespace postgres
#endif // POSTGRES_CONNECTION_HPP
