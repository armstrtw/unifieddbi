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

## compatibility function to forward dbyType to dbConnect
## as in with R/S DBI:
## conn <- dbConnect(dbDriver("PostgreSQL"), user="the.user", dbname="the.dbname")
dbDriver <- function(dbType) {
    dbType
}

dbConnect <- function(dbType, connection.string=NULL, user=NULL, pass=NULL, host=NULL, port=NULL, tty=NULL, dbname=NULL, options=NULL) {
  .Call("dbConnect", dbType, connection.string, user, pass, host, port, tty, dbname, options, PACKAGE="unifiedDBI")
}

dbSendQuery <- function(dbi.conn,qry) {
    .Call("dbSendQuery", dbi.conn, qry, PACKAGE="unifiedDBI")
}

fetch <- function(dbi.query.results, nrows= -1L) {
    .Call("dbfetch", dbi.query.results, as.integer(nrows), PACKAGE="unifiedDBI")
}

dbFetchQuery <- function(dbi.conn,qry) {
    res <- dbSendQuery(dbi.conn,qry)
    ans <- fetch(res)
    dbClearResult(res)
    ans
}

dbClearResult <- function(dbi.query.results) {
    .Call("dbClearResult", dbi.query.results, PACKAGE="unifiedDBI")
}

dbDisconnect <- function(dbi.conn) {
    .Call("dbDisconnect", dbi.conn, PACKAGE="unifiedDBI")
}

dbWriteTable <- function(dbi.conn, tableName, value, row.names=FALSE, overwrite=FALSE, append=FALSE) {
    .Call("dbWriteTable", dbi.conn, tableName, value, row.names, overwrite, append, PACKAGE="unifiedDBI")
}

dbExistsTable <- function(dbi.conn, tableName) {
    .Call("dbExistsTable", dbi.conn, tableName, PACKAGE="unifiedDBI")
}

dbListTables <- function(dbi.conn) {
    .Call("dbListTables", dbi.conn, PACKAGE="unifiedDBI")
}

## create an index on database "db.name" using the columns "cols"
dbCreateIndex <- function(conn,db.name,cols) {
    idx.name <- gsub("\\.","_",paste(db.name,paste(cols,collapse="_"),"idx",sep="_"))
    idx.definition <- paste(db.name,"(",paste(cols,collapse=","),")",sep="")
    idx.command <- paste("CREATE INDEX",idx.name,"on",idx.definition)
    dbSendQuery(dev.conn,idx.command)
}
