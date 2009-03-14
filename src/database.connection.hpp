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

#ifndef DATABASE_CONNECTION_HPP
#define DATABASE_CONNECTION_HPP


#include <string>
#include <vector>
#include <stdexcept>
#include <Rinternals.h>

#include "query.results.hpp"

class DatabaseConnection {
public:
  virtual ~DatabaseConnection() {}
  virtual void connect(const char* connectionString) = 0;

  virtual void disConnect() = 0;
  virtual bool commit() = 0;
  virtual bool rollback() = 0;
  virtual SEXP listTables() = 0;
  virtual SEXP readTable(const char* tableName) = 0;
  virtual int writeTable(const char* tableName, SEXP x, const bool writeRowNames, const bool overwrite, const bool append) = 0;
  virtual bool existsTable(const char* tableName) = 0;
  virtual bool removeTable(const char* tableName) = 0;
  virtual QueryResults* sendQuery(const char* query) = 0;

  class BadDatabaseConnection : public std::logic_error {
  public:
    BadDatabaseConnection(const char* conn_error)
      : std::logic_error(std::string("Database connection error: ") + std::string(conn_error)) {}
  };

  class DriverNotSupported : public std::logic_error {
  public:
    DriverNotSupported(const char* dbType)
      : std::logic_error(std::string("Cannot create database driver of type: ") + std::string(dbType)) {}
  };

  // factory method:
  static DatabaseConnection* init(const char* dbType);
};


#endif // DATABASE_CONNECTION_HPP
