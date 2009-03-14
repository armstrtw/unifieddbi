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

#ifndef POSTGRES_COLUMN_FACTORY_HPP
#define POSTGRES_COLUMN_FACTORY_HPP

#include <iostream>
#include "column.factory.hpp"
#include "postgres.results.column.hpp"

namespace postgres {

  class PostgresColumnFactory : public ColumnFactory {
  private:
    const PGresult *res_;
    PostgresResultColumn* createColumn(const int i) const;
  public:
    PostgresColumnFactory(const PGresult *res);
    void init(std::vector<QueryResultColumn*>& ans) const;
  };

} // namespace postgres
#endif // POSTGRES_COLUMN_FACTORY_HPP		\
