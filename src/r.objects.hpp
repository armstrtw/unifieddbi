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

#ifndef R_OBJECTS_HPP
#define R_OBJECTS_HPP

#include <string>
#include <Rinternals.h>
#include "database.connection.hpp"

class Robject {
protected:
  SEXP sexp_;
public:
  Robject(SEXP x) : sexp_(x) {}
  ~Robject() {}
  static Robject* factory(SEXP x);
  virtual int writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames) = 0;
};

class RDataFrame : public Robject {
private:
friend class Robject;
  RDataFrame(SEXP x) : Robject(x) {}
public:
  bool hasRownames() const;
  bool hasColnames() const;
  SEXP getRownames() const;
  SEXP getColnames() const;
  int writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames);
};


class RMatrix : public Robject {
private:
  friend class Robject;
  RMatrix(SEXP x) : Robject(x) {}
public:
  bool hasRownames() const;
  bool hasColnames() const;
  SEXP getRownames() const;
  SEXP getColnames() const;
  int writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames);
};

class RVector : public Robject {
private:
  friend class Robject;
  RVector(SEXP x) : Robject(x) {}
public:
  bool hasNames() const;
  SEXP getNames() const;
  int writeToDatabase(DatabaseConnection* conn, const char* tableName, const bool writeNames);
};

#endif // R_OBJECTS_HPP
