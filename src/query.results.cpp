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

  SEXP ans = PROTECT(allocVector(VECSXP,ncols));
  for(R_len_t i = 0; i < ncols; i++) {
    SEXP resultsSEXP = PROTECT(queryResultColumns_[i]->allocateSEXP(nrows));
    SET_VECTOR_ELT(ans,i,resultsSEXP);
    UNPROTECT(1);
  }

  // the order of the attributes matters for testing w/ all.equal
  // needs to be (names,row.names,class)

  // names
  getColnames(colnames);
  SEXP ans_cnames = PROTECT(string2sexp(colnames.begin(),colnames.end()));
  namesgets(ans,ans_cnames);
  UNPROTECT(1); // ans_cnames

  // rownames
  // R now allows integer rownames (as of 3.0.2)
  // test case: x <- data.frame(a=1:10,b=11:20); sapply(attributes(x),class)
  addFakeRownames(ans, nrows);

  // class attribute
  SEXP klass = PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(klass, 0, mkChar("data.frame"));
  classgets(ans,klass);
  UNPROTECT(1); // klass

  UNPROTECT(1); // ans
  return ans;
}


SEXP QueryResults::fetch(const int fetch_rows) {
  //ignore fetch_rows for now
  const int ncols = ncol();
  const int nrows = nrow();
  SEXP ans;
  PROTECT(ans = allocSEXP(nrows));

  // pull values
  // this is mean to be as flexible as possible
  // since different db's might need full access to res
  // for instance postgres needs res to do a null test
  // on the tuple value
  for(int row = 0; row < nrows; row++) {
    for(int col = 0; col < ncols; col++) {
      queryResultColumns_[col]->setValue(VECTOR_ELT(ans,col), row);
    }
  }
  UNPROTECT(1); // ans
  return ans;
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
