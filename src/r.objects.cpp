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

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "r.objects.hpp"
#include "Rutilities.hpp"

using std::vector;
using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;


Robject* Robject::factory(SEXP x) {
  SEXP r_class = getAttrib(x,R_ClassSymbol);
  const std::string r_class_str(CHAR(STRING_ELT(r_class,0)));
  if(r_class == R_NilValue) {
    //throw()
    return NULL;
  }
  if(r_class_str == "matrix") {
    return new RMatrix(x);
  }

  if(r_class_str == "data.frame") {
    return new RDataFrame(x);
  }

  return new RVector(x);
}

bool RDataFrame::hasRownames() const {
  return (getRownames() == R_NilValue) ? false : true;
}

bool RDataFrame::hasColnames() const {
  return (getColnames() == R_NilValue) ? false : true;
}

SEXP RDataFrame::getRownames() const {
  return getAttrib(sexp_, install("row.names"));
}

SEXP RDataFrame::getColnames() const {
  return getAttrib(sexp_, R_NamesSymbol);
}

void RDataFrame::getColnames(std::vector<std::string>& ans) const {
  sexp2string(getColnames(),back_inserter(ans));
}

void RDataFrame::getSEXPS(std::vector<SEXP>& ans) const {
  for(R_len_t i = 0; i < length(sexp_); i++) {
    ans.push_back(VECTOR_ELT(sexp_,i));
  }
}


R_len_t RDataFrame::nrow() const {
  if(sexp_==R_NilValue) {
    return 0;
  }
  return length(VECTOR_ELT(sexp_,0));
}

R_len_t RDataFrame::ncol() const {
  if(sexp_==R_NilValue) {
    return 0;
  }
  return length(sexp_);
}

R_len_t RDataFrame::getOffset(const R_len_t colNum) const {
  return 0;
}

bool RMatrix::hasRownames() const {
  return (getRownames() == R_NilValue) ? false : true;
}

bool RMatrix::hasColnames() const {
  return (getColnames() == R_NilValue) ? false : true;
}

SEXP RMatrix::getRownames() const {
  SEXP dimnames = getAttrib(sexp_, R_DimNamesSymbol);

  if(dimnames==R_NilValue) {
    return R_NilValue;
  }

  return VECTOR_ELT(dimnames, 0);
}

SEXP RMatrix::getColnames() const {
 SEXP dimnames = getAttrib(sexp_, R_DimNamesSymbol);

  if(dimnames==R_NilValue) {
    return R_NilValue;
  }
  return VECTOR_ELT(dimnames, 1);
}

void RMatrix::getColnames(std::vector<std::string>& ans) const {
  sexp2string(getColnames(),back_inserter(ans));
}

void RMatrix::getSEXPS(std::vector<SEXP>& ans) const {
  for(R_len_t i = 0; i < length(sexp_); i++) {
    ans.push_back(sexp_);
  }
}

R_len_t RMatrix::nrow() const {
  if(sexp_==R_NilValue) {
    return 0;
  }

  return nrows(sexp_);
}

R_len_t RMatrix::ncol() const {
  if(sexp_==R_NilValue) {
    return 0;
  }
  return ncols(sexp_);
}

R_len_t RMatrix::getOffset(const R_len_t colNum) const {
  return colNum * nrow();
}

bool RVector::hasRownames() const {
  return (getRownames() == R_NilValue) ? false : true;
}

bool RVector::hasColnames() const {
  return false;
}

SEXP RVector::getColnames() const {
  return R_NilValue;
}

SEXP RVector::getRownames() const {
  return getAttrib(sexp_, R_NamesSymbol);
}

void RVector::getColnames(std::vector<std::string>& ans) const {
  ans.push_back("r_vector");
}

void RVector::getSEXPS(std::vector<SEXP>& ans) const {
    ans.push_back(sexp_);
}

R_len_t RVector::nrow() const {
  if(sexp_==R_NilValue) {
    return 0;
  }
  return length(sexp_);
}

R_len_t RVector::ncol() const {
  if(sexp_==R_NilValue) {
    return 0;
  }
  return 1;
}

R_len_t RVector::getOffset(const R_len_t colNum) const {
  return 0;
}
