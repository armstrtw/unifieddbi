#include "query.results.hpp"

QueryResults::~QueryResults() {
  for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter != queryResultColumns_.end(); iter++) {
    delete *iter;
  }
}

// use nrows as parameter b/c user might not want full
// results set of query i.e call to fetch(res, n = 100)
SEXP QueryResults::allocSEXP(const R_len_t nrows) const {
  SEXP ans;
  const int ncols = ncol();

  PROTECT(ans = allocVector(VECSXP, ncols));
  //for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter != queryResultColumns_.end(); iter++) {
  for(R_len_t i = 0; i < ncols; i++) {
    SET_VECTOR_ELT(ans,i,queryResultColumns_[i]->allocateSEXP(nrows));
  }
  UNPROTECT(1);
  return ans;
}


SEXP QueryResults::fetch(const int fetch_rows) {
  SEXP ans;

  //ignore fetch_rows for now
  const int ncols = ncol();
  const int nrows = nrow();
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
  // set colnames, other attributes...
  UNPROTECT(1);
  return ans;
}

void QueryResults::getColnames(std::vector<std::string>& ans) const {
  // for(std::vector<QueryResultColumn*>::iterator iter = queryResultColumns_.begin(); iter!= queryResultColumns_.end(); iter++) {
  //   ans.push_back((*iter)->getName());
  // }
  for(R_len_t i = 0; i < ncol(); i++) {
    ans.push_back(queryResultColumns_[i]->getName());
  }
}
