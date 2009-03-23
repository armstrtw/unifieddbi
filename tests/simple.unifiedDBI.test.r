library(unifiedDBI)

gen.test.data <- function() {
  test.dates.POSIXct <- as.POSIXct(seq.POSIXt(as.POSIXct("2000-01-01"),as.POSIXct("2050-01-01"),by="DSTday"))
  ans.nrow <- length(test.dates.POSIXct)

  test.dates.POSIXlt <- as.POSIXlt(test.dates.POSIXct)
  test.double <- rnorm(ans.nrow)
  test.int <- as.integer(test.double*10000)
  test.factor <- as.factor(rep(c("cat","bat","fat","hat"),length.out=ans.nrow))
  test.character <- rep(c("big","black","dog"),length.out=ans.nrow)

  data.frame(test.dates.POSIXct=test.dates.POSIXct,
             test.dates.POSIXlt=test.dates.POSIXlt,
             test.double=test.double,
             test.int=test.int,
             test.factor=test.factor,
             test.character=test.character)
}

## test all new implementations by passing in a connection object here
test.function <- function(conn,the.test.data) {
  test.data.rows <- nrow(the.test.data)
  cat("writing",test.data.rows,"rows.\n")
  print(write.time <- system.time(rows.written <- dbWriteTable(conn,"test_table",the.test.data,overwrite=TRUE)))
  stopifnot(test.data.rows == rows.written)
  cat("write speed (rows/second):",test.data.rows/write.time[1:3],"\n")

  res <- dbSendQuery(conn,"select * from test_table")
  cat("fetching",test.data.rows,"rows.\n")
  print(read.time <- system.time(ans <- fetch(res)))
  dbClearResult(res)
  stopifnot(test.data.rows == nrow(ans))
  cat("read speed (rows/second):",test.data.rows/read.time[1:3],"\n")

  test.data.for.comparison <- test.data
  test.data.for.comparison[,"test.dates.POSIXlt"] <- as.POSIXct(test.data.for.comparison[,"test.dates.POSIXlt"])
  test.data.for.comparison[,"test.factor"] <- as.character(test.data.for.comparison[,"test.factor"])

  stopifnot(all.equal(ans,test.data.for.comparison))

  browser()
  dbDisconnect(conn)
}

test.data <- gen.test.data()
conn <- dbConnect("PostgreSQL","user=admin dbname=netflix")
test.function(conn,test.data)
