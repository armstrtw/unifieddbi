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

#ifndef COLUMN_FOR_WRITING_HPP
#define COLUMN_FOR_WRITING_HPP

#include <Rinternals.h>

////////////////////////////////////////////////////////////////////////////
//    encompasses a column to be written to the database                  //
//    i.e. it holds the proper offset for matrix types in R               //
//    in which different columns are represented as offsets from the same //
//    base memory address                                                 //
////////////////////////////////////////////////////////////////////////////
class ColumnForWriting {
public:
  SEXP sexp;
  R_len_t offset;
  //std::string colname_;
  ColumnForWriting(const SEXP sexp_, const R_len_t offset_) : sexp(sexp_), offset(offset_) {}
};


#endif // COLUMN_FOR_WRITING_HPP
