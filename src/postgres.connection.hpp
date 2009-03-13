#ifndef POSTGRES_CONNECTION_HPP
#define POSTGRES_CONNECTION_HPP

#include <libpq-fe.h>
#include <Rinternals.h>

#include "database.connection.hpp"
#include "postgres.results.hpp"


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

#endif // POSTGRES_CONNECTION_HPP
