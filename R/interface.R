##########################################################################/
## Copyright (C) 2009  Whit Armstrong                                    ##
##                                                                       ##
## This program is free software: you can redistribute it and/or modify  ##
## it under the terms of the GNU General Public License as published by  ##
## the Free Software Foundation, either version 3 of the License, or     ##
## (at your option) any later version.                                   ##
##                                                                       ##
## This program is distributed in the hope that it will be useful,       ##
## but WITHOUT ANY WARRANTY; without even the implied warranty of        ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         ##
## GNU General Public License for more details.                          ##
##                                                                       ##
## You should have received a copy of the GNU General Public License     ##
## along with this program.  If not, see <http://www.gnu.org/licenses/>. ##
###########################################################################

dbDriver <- function(dbType) {
    dbType
}

dbConnect <- function(dbType, connection_string) {
    .Call("dbConnect",dbType, connection_string, PACKAGE="unifiedDBI")
}

dbSendQuery <- function(dbi_conn,qry) {
    .Call("dbSendQuery", dbi_conn, qry, PACKAGE="unifiedDBI")
}

fetch <- function(dbi_query_results, nrows= -1L) {
    .Call("dbfetch", dbi_query_results, as.integer(nrows), PACKAGE="unifiedDBI")
}


dbClearResult <- function(dbi_query_results) {
    .Call("dbClearResult", dbi_query_results, PACKAGE="unifiedDBI")
}
