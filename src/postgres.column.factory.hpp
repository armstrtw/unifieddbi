#ifndef POSTGRES_COLUMN_FACTORY_HPP
#define POSTGRES_COLUMN_FACTORY_HPP

#include <iostream>
#include "column.factory.hpp"
#include "postgres.results.column.hpp"

class PostgresColumnFactory : public ColumnFactory {
private:
  const PGresult *res_;
  PostgresResultColumn* createColumn(const int i) const;
public:
  PostgresColumnFactory(const PGresult *res);
  void init(std::vector<QueryResultColumn*>& ans) const;
};

#endif // POSTGRES_COLUMN_FACTORY_HPP
