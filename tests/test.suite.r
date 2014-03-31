library(unifiedDBI)

gen.test.data <- function() {
    ## long data
    test.dates.posixct <- seq.POSIXt(as.POSIXct("2000-01-01"),as.POSIXct("2050-01-01"),by="DSTday")

    ## short data
    ##test.dates.posixct <- seq.POSIXt(as.POSIXct("2000-01-01"),as.POSIXct("2000-01-31"),by="DSTday")
    ans.nrow <- length(test.dates.posixct)

    test.double.dates <- seq(as.Date("2000-01-01"),length.out=ans.nrow,by="day"); storage.mode(test.double.dates) <- "double"
    test.int.dates <- seq(as.Date("2000-01-01"),length.out=ans.nrow,by="day")
    test.seconds.POSIXct <- seq.POSIXt(as.POSIXct("2000-01-01"),length.out=ans.nrow,by="sec")
    test.dates.posixlt <- as.POSIXlt(test.dates.posixct)
    test.double <- rnorm(ans.nrow)
    test.int <- as.integer(test.double*10000)
    test.factor <- as.factor(rep(c("cat","bat","fat","hat"),length.out=ans.nrow))
    test.character <- rep(c("big","black","dog"),length.out=ans.nrow)
    test.logical <- as.logical(rnorm(ans.nrow) > .2)

    ans <- data.frame(##test.dates.posixct=test.dates.posixct,
                      test.double.dates=test.double.dates,
                      test.int.dates=test.int.dates,
                      ##test.seconds.POSIXct=test.seconds.POSIXct,
                      ##test.dates.posixlt=test.dates.posixlt,
                      test.double=test.double,
                      test.int=test.int,
                      test.factor=test.factor,
                      test.character=I(test.character),
                      ##test.character=test.character,
                      test.logical=test.logical)

    ## get rid of 'AsIs' class
    class(ans[,"test.character"]) <- NULL

    ans
}

## test all new implementations by passing in a connection object here
test.function <- function(conn,the.test.data) {
    test.data.rows <- nrow(the.test.data)
    cat("writing",test.data.rows,"rows.\n")
    print(write.time <- system.time(rows.written <- dbWriteTable(conn,"test_table",the.test.data,overwrite=TRUE)))
    stopifnot(test.data.rows == rows.written)
    cat("write speed (rows/second):",test.data.rows/write.time["elapsed"],"\n")

    cat("fetching",test.data.rows,"rows.\n")
    print(read.time <- system.time(ans <- dbFetchQuery(conn,"select * from test_table")))
    stopifnot(test.data.rows == nrow(ans))
    cat("read speed (rows/second):",test.data.rows/read.time["elapsed"],"\n")

    test.data.for.comparison <- the.test.data    
    ##test.data.for.comparison[,"test.dates.posixlt"] <- as.POSIXct(test.data.for.comparison[,"test.dates.posixlt"])
    storage.mode(test.data.for.comparison[,"test.double.dates"]) <- "integer"
    test.data.for.comparison[,"test.factor"] <- as.character(test.data.for.comparison[,"test.factor"])

    browser()
    
    stopifnot(all.equal(colnames(test.data.for.comparison),colnames(ans)))
    stopifnot(all.equal(lapply(ans,class),lapply(test.data.for.comparison,class)))
    stopifnot(all.equal(lapply(ans,storage.mode),lapply(test.data.for.comparison,storage.mode)))
    stopifnot(all.equal(ans,test.data.for.comparison))
    dbDisconnect(conn)
}

test.data <- gen.test.data()
conn <- dbConnect("PostgreSQL","host=mongodb user=dbadmin dbname=kls")
test.function(conn,test.data)
