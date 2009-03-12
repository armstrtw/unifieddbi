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

  if(dbType_s == "POSTGRES") {
    return new PostgresConnection();
  } else {
    cerr << "database type not found." << endl;
    return static_cast<DatabaseConnection*>(NULL);
  }
}
