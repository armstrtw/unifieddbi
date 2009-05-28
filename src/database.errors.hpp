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

#ifndef DATABASE_ERRORS_HPP
#define DATABASE_ERRORS_HPP

#include <stdexcept>
#include "r.column.types.hpp"

  // all implementations should use these errors
  class BadDatabaseConnection : public std::logic_error {
  public:
    BadDatabaseConnection(const char* conn_error);
  };

  class DriverNotSupported : public std::logic_error {
  public:
    DriverNotSupported(const char* dbType);
  };

  class TypeNotSupported : public std::logic_error {
  public:
    TypeNotSupported(const char* RObject_Type);
  };

  class MapToTypeNotImplemented : public std::logic_error {
  public:
    MapToTypeNotImplemented(const std::string& RType, const std::string& dbType);
  };

class unknownRColumnType : public std::logic_error {
 public:
  unknownRColumnType(RColumnType dbColType);
};

#endif // DATABASE_ERRORS_HPP
