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

BadDatabaseConnection::BadDatabaseConnection(const char* conn_error)
  : std::logic_error(std::string("Database connection error: ") + std::string(conn_error)) {}

DriverNotSupported::DriverNotSupported(const char* dbType)
  : std::logic_error(std::string("Cannot create database driver of type: ") + std::string(dbType)) {}


TypeNotSupported::TypeNotSupported(const char* RObject_Type)
  : std::logic_error(std::string("Cannot convert R object of class/type: ") + std::string(RObject_Type)) {}

// FIXME: add another arg which is R object type, so we can see the exact mapping routine that
// needs to be built
MapToTypeNotImplemented::MapToTypeNotImplemented(const char* dbType)
  : std::logic_error(std::string("Cannot convert R object to database type: ") + std::string(dbType)) {}

