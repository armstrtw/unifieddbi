#include <iostream>
#include "postgres.column.factory.hpp"
#include "postgres.oid.hpp"
#include "postgres.results.column.hpp"

using std::cout;
using std::cerr;
using std::endl;

PostgresColumnFactory::PostgresColumnFactory(const PGresult *res): res_(res) {}

PostgresResultColumn* PostgresColumnFactory::createColumn(const int i) const {
  switch(static_cast<pg_oidT>(PQftype(res_,i))) {
  case INT4OID:
  case INT8OID:
  case OIDOID:
    return new int_char(res_, i);
  case FLOAT4OID:
  case FLOAT8OID:
    return new float_char(res_, i);
  case DATEOID:
    return new date_char(res_, i);
  case TEXTOID:
    return new text_char(res_, i);
  default:
    cerr << "conversion not supported using null conversion for column number: " << i << endl;
    return new defaultResultColumn(res_, i);
  }
}

void PostgresColumnFactory::init(std::vector<QueryResultColumn*>& ans) const {
  const int ncols = PQnfields(res_);
  for(int i = 0; i < ncols; i++) {
    cout << "PostgresColumnFactory: init column: " << i << endl;
    ans.push_back(createColumn(i));
  }
}

