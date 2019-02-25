library(unifiedDBI)

gen.test.data <- function() {
  test.dates <- seq.Date(as.Date("2000-01-01"),as.Date("2050-01-01"),by="day")
  ans.nrow <- length(test.dates)

  data.frame(test.dates=test.dates,
             test.double=rnorm(ans.nrow),
             test.int=as.integer(runif(ans.nrow,-100,100)),
             test.factor=as.factor(rep(c("cat","bat","fat","hat"),length.out=ans.nrow)),
             test.character=rep(c("big","black","dog"),length.out=ans.nrow),
             stringsAsFactors=FALSE
            )
}

## test all new implementations by passing in a connection object here
test.function <- function(conn,the.test.data) {
  test.data.rows <- nrow(the.test.data)
  cat("writing",test.data.rows,"rows.\n")
  print(write.time <- system.time(rows.written <- dbWriteTable(conn,"trading_systems.test_table",the.test.data,overwrite=TRUE)))
  stopifnot(test.data.rows == rows.written)
  cat("write speed (rows/second):",test.data.rows/write.time[1:3],"\n")

  res <- dbSendQuery(conn,"select * from trading_systems.test_table")
  cat("fetching",test.data.rows,"rows.\n")
  print(read.time <- system.time(ans <- fetch(res)))
  dbClearResult(res)
  stopifnot(test.data.rows == nrow(ans))
  cat("read speed (rows/second):",test.data.rows/read.time[1:3],"\n")

  ## factor read back as character
  ans$test.factor <- I(as.factor(ans$test.factor))

  stopifnot(all.equal(ans,the.test.data,check.attributes=FALSE))

  dbDisconnect(conn)
}

test.data <- gen.test.data()
conn <- dbConnect(dbDriver("PostgreSQL"),connection.string="host=db port=5432 dbname=kls user=rates")
test.function(conn,test.data)
