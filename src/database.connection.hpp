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

#include "type.converter.hpp"
#include "query.results.hpp"

class DatabaseConnection {
public:
  virtual ~DatabaseConnection() {}

  // virtuals
  virtual void connect(const char* connectionString) = 0;
  virtual void connect(const char* user,
		       const char* pass,
		       const char* host,
		       const char* port,
		       const char* tty,
		       const char* dbName,
		       const char* options) = 0;

  virtual void disConnect() = 0;
  virtual bool commit() = 0;
  virtual bool rollback() = 0;
  virtual SEXP listTables() = 0;
  virtual bool existsTable(const char* tableName) = 0;
  virtual QueryResults* sendQuery(const char* query) = 0;
  virtual TypeConverter* getTypeConverter(SEXP x) = 0;

  // implemented in this class
  bool removeTable(const char* tableName);
  SEXP readTable(const char* tableName);

  // specific implementations can define writeTable
  // this implementation will use text conversions: "insert into _table_ values (1,2,3)"
  // where the rows are converted to text using TypeConverters
  virtual int writeTable(const char* tableName, SEXP value_sexp, const bool writeRowNames);
  // do not delete createTableFromTypes
  void createTable(const char* tableName, std::vector<std::string>& colnames, std::vector<TypeConverter*>& typeConverters);

  // factory method:
  static DatabaseConnection* init(const char* dbType);

  // all implementations should use these errors
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
  class TypeNotSupported : public std::logic_error {
  public:
    TypeNotSupported(const char* RObject_Type)
      : std::logic_error(std::string("Cannot convert R object of class/type: ") + std::string(RObject_Type)) {}
  };
};


#endif // DATABASE_CONNECTION_HPP
