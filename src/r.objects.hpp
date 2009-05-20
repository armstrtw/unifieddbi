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
#include <vector>
#include <Rinternals.h>

#include "column.for.writing.hpp"

class Robject {
protected:
  SEXP sexp_;
public:
  static Robject* factory(SEXP x);
  Robject(SEXP x) : sexp_(x) {}
  virtual ~Robject() {}
  virtual R_len_t nrow() const = 0;
  virtual R_len_t ncol() const = 0;
  virtual bool hasRownames() const = 0;
  virtual bool hasColnames() const = 0;
  virtual SEXP getRownames() const = 0;
  virtual SEXP getColnames() const = 0;
  virtual void getColnames(std::vector<std::string>& ans) const = 0;
  virtual void getSEXPS(std::vector<SEXP>& ans) const = 0;
  virtual void createWriteJob(std::vector<ColumnForWriting>& write_job, const bool writeRowNames) const = 0;
};

class RDataFrame : public Robject {
private:
friend class Robject;
  RDataFrame(SEXP x) : Robject(x) {}
public:
  R_len_t nrow() const;
  R_len_t ncol() const;
  bool hasRownames() const;
  bool hasColnames() const;
  SEXP getRownames() const;
  SEXP getColnames() const;
  void getColnames(std::vector<std::string>& ans) const;
  void getSEXPS(std::vector<SEXP>& ans) const;
  void createWriteJob(std::vector<ColumnForWriting>& write_job, const bool writeRowNames) const;
};


class RMatrix : public Robject {
private:
  friend class Robject;
  RMatrix(SEXP x) : Robject(x) {}
public:
  R_len_t nrow() const;
  R_len_t ncol() const;
  bool hasRownames() const;
  bool hasColnames() const;
  SEXP getRownames() const;
  SEXP getColnames() const;
  void getColnames(std::vector<std::string>& ans) const;
  void getSEXPS(std::vector<SEXP>& ans) const;
  void createWriteJob(std::vector<ColumnForWriting>& write_job, const bool writeRowNames) const;
};

class RVector : public Robject {
private:
  friend class Robject;
  RVector(SEXP x) : Robject(x) {}
public:
  R_len_t nrow() const;
  R_len_t ncol() const;
  bool hasRownames() const;
  bool hasColnames() const;
  SEXP getRownames() const;
  SEXP getColnames() const;
  void getColnames(std::vector<std::string>& ans) const;
  void getSEXPS(std::vector<SEXP>& ans) const;
  void createWriteJob(std::vector<ColumnForWriting>& write_job, const bool writeRowNames) const;
};

#endif // R_OBJECTS_HPP
