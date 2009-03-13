#ifndef COLUMN_FACTORY_HPP
#define COLUMN_FACTORY_HPP

#include <vector>
#include "query.results.column.hpp"

class ColumnFactory {
public:
  virtual void init(std::vector<QueryResultColumn*>& ans) const = 0;
  virtual ~ColumnFactory() {}
};

#endif // COLUMN_FACTORY_HPP
