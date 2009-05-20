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

#include "column.for.writing.hpp"
#include "query.results.hpp"
#include "r.column.types.hpp"

class DatabaseConnection {
private:
  void fixColnames(std::vector<std::string>& colnames);
protected:
  virtual std::string getBooleanType() const = 0;
  virtual std::string getIntegerType() const = 0;
  virtual std::string getDoubleType() const = 0;
  virtual std::string getCharacterType() const = 0;
  virtual std::string getFactorType() const = 0;
  virtual std::string getDateTimeType() const = 0;
  virtual std::string getDateType() const = 0;
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
  virtual bool transaction_begin() = 0;
  virtual bool rollback() = 0;
  virtual bool commit() = 0;
  virtual SEXP listTables() = 0;
  virtual bool existsTable(const char* tableName) = 0;
  virtual QueryResults* sendQuery(const char* query) = 0;
  virtual int write(const char * tableName, const std::vector<ColumnForWriting>& cols, const int nrows) = 0;

  // implemented in this class
  bool removeTable(const char* tableName);
  SEXP readTable(const char* tableName);
  void createTable(const char* tableName, SEXP value_sexp, const bool writeRowNames);
  std::string getNativeType(RColumnType);
  int writeTable(const char* tableName, SEXP value_sexp, const bool writeRowNames);

  // factory method:
  static DatabaseConnection* init(const char* dbType);
};


#endif // DATABASE_CONNECTION_HPP
