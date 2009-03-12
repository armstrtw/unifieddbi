#ifndef POSTGRES_CONNECTION_HPP
#define POSTGRES_CONNECTION_HPP

#include <libpq-fe.h>
#include <Rinternals.h>

#include "database.connection.hpp"
#include "postgres.results.hpp"


class PostgresConnection : public DatabaseConnection {
private:
  PGconn* conn_;

  PostgresConnection() {}
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


// conParams->user = RS_DBI_copyString(PQuser(my_connection));
// conParams->password = RS_DBI_copyString(PQpass(my_connection));
// conParams->host = RS_DBI_copyString(PQhost(my_connection));
// conParams->dbname = RS_DBI_copyString(PQdb(my_connection));
// conParams->port = RS_DBI_copyString(PQport(my_connection));
// conParams->tty = RS_DBI_copyString(PQtty(my_connection));
// conParams->options = RS_DBI_copyString(PQoptions(my_connection));

// Con_Handle *
// RS_MySQL_newConnection(Mgr_Handle *mgrHandle, 
//     s_object *s_dbname,
//     s_object *s_username,
//     s_object *s_password,
//     s_object *s_myhost,
//     s_object *s_unix_socket,
//     s_object *s_port,
//     s_object *s_client_flag,
//     s_object *s_groups,
//     s_object *s_default_file)
// {


#endif // POSTGRES_CONNECTION_HPP
