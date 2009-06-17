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

#include <string>
#include "r.objects.hpp"
#include "create.write.job.hpp"
#include "utils.hpp"

using std::vector;
using std::string;

// FIXME: add null check here or in robject factory

void createWriteJob(const Robject* x, std::vector<ColumnForWriting>& write_job, const bool writeRowNames) {
  vector<string> colnames;
  vector<SEXP> sexps;

  if(writeRowNames && x->hasRownames()) {
    write_job.push_back(ColumnForWriting(x->getRownames(),0,"rownames"));
  }

  // FIXME: fail/throw if there are no colnames
  x->getSEXPS(sexps);
  x->getColnames(colnames);
  fixColnames(colnames);
  for(R_len_t i = 0; i < x->ncol(); i++) {
    write_job.push_back(ColumnForWriting(sexps[i],x->getOffset(i),colnames[i]));
  }
}
