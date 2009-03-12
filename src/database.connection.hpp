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
  virtual void listTables(std::vector<std::string>& ans) = 0;
  virtual SEXP readTable(const char* tableName) = 0;
  virtual void writeTable(const char* tableName, SEXP x, const bool writeRowNames, const bool overwrite, const bool append) = 0;
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
