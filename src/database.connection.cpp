#include <iostream>
#include "database.connection.hpp"
#include "postgres.connection.hpp"

using std::string;
using std::cerr;
using std::endl;

DatabaseConnection* DatabaseConnection::init(const char* dbType) {
  if(dbType == NULL) {
    // throw
    return NULL;
  }

  const string dbType_s(dbType);

  if(dbType_s == "POSTGRES" || dbType_s == "PostgreSQL") {
    return new PostgresConnection();
  } else {
    throw DatabaseConnection::DriverNotSupported(dbType_s.c_str());
  }
}
