dbConnect <- function(dbType, connection_string) {
    .Call("dbConnect",dbType, connection_string, PACKAGE="unifiedDBI")
}

dbSendQuery <- function(dbi_conn,qry) {
    .Call("dbSendQuery", dbi_conn, qry, PACKAGE="unifiedDBI")
}

dbfetch <- function(dbi_query_results, nrows= -1L) {
    .Call("dbfetch", dbi_query_results, as.integer(nrows), PACKAGE="unifiedDBI")
}


dbClearResult <- function(dbi_query_results) {
    .Call("dbClearResult", dbi_query_results, PACKAGE="unifiedDBI")
}
