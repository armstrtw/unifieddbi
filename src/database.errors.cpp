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

#include "database.errors.hpp"
#include "conversion.utils.hpp"

using std::string;

BadDatabaseConnection::BadDatabaseConnection(const char* conn_error)
  : std::logic_error(string("Database connection error: ") + string(conn_error)) {}

DriverNotSupported::DriverNotSupported(const char* dbType)
  : std::logic_error(string("Cannot create database driver of type: ") + string(dbType)) {}


TypeNotSupported::TypeNotSupported(const char* RObject_Type)
  : std::logic_error(string("Cannot convert R object of class/type: ") + string(RObject_Type)) {}

// FIXME: add another arg which is R object type, so we can see the exact mapping routine that
// needs to be built
MapToTypeNotImplemented::MapToTypeNotImplemented(const string& RType, const string& dbType)
  : std::logic_error(string("Cannot create map to type: ") + string(RType) + " " + string(dbType)) {}

unknownRColumnType::unknownRColumnType(RColumnType dbColType)
  : std::logic_error(string("Unknown RColumnType ") + itos(dbColType)) {}

unknownRelationName::unknownRelationName(const string& relationName)
  : std::logic_error(string("Unknown relation name: ") + relationName) {}
