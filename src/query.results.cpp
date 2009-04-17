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
#include <string>
#include <Rvector.hpp>
#include "query.results.hpp"
#include "conversion.utils.hpp"
#include "utils.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using namespace RAbstraction;

QueryResults::QueryResults(ColumnFactory* columnFactory): columnFactory_(columnFactory), completedRows_(0) {
  columnFactory_->init(queryResultColumns_);
}

QueryResults::~QueryResults() {
  delete columnFactory_;
  for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter != queryResultColumns_.end(); iter++) {
    delete *iter;
  }
}

// use nrows as parameter b/c user might not want full
// results set of query i.e call to fetch(res, n = 100)
SEXP QueryResults::allocSEXP(const R_len_t nrows) const {
  vector<string> colnames;
  vector<string> rownames(nrows);

  const int ncols = ncol();

  RVector<VECSXP> ans(ncols);

  //for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter != queryResultColumns_.end(); iter++) {
  for(R_len_t i = 0; i < ncols; i++) {
    SET_VECTOR_ELT(ans.getSEXP(),i,queryResultColumns_[i]->allocateSEXP(nrows));
  }
  ans.setClass("data.frame");
  getColnames(colnames);
  ans.setNames(colnames.begin(),colnames.end());
  for(R_len_t i = 0; i < nrows; i++) { rownames[i] = itos(i+1); }
  ans.setAttribute("row.names",string2sexp(rownames.begin(),rownames.end()));
  return ans.getSEXP();
}


SEXP QueryResults::fetch(const int fetch_rows) {
  //ignore fetch_rows for now
  const int ncols = ncol();
  const int nrows = nrow();
  RVector<VECSXP> ans(allocSEXP(nrows));

  // pull values
  // this is mean to be as flexible as possible
  // since different db's might need full access to res
  // for instance postgres needs res to do a null test
  // on the tuple value
  for(int row = 0; row < nrows; row++) {
    for(int col = 0; col < ncols; col++) {
      queryResultColumns_[col]->setValue(VECTOR_ELT(ans.getSEXP(),col), row);
    }
  }
  return ans.getSEXP();
}

void QueryResults::getColnames(std::vector<std::string>& ans) const {
  // for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter!= queryResultColumns_.end(); iter++) {
  //   ans.push_back((*iter)->getName());
  // }
  for(R_len_t i = 0; i < ncol(); i++) {
    ans.push_back(queryResultColumns_[i]->getName());
  }
  cleanColnames(ans, "_",".");
}

const int QueryResults::ncol() const {
  return static_cast<const int>(queryResultColumns_.size());
}
