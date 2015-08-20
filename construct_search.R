args <- commandArgs(trailingOnly=TRUE)
if (length(args)!=2) stop("Need args: dataset Rname")
if (!file.exists(args[1])) stop("No file!")

e <- new.env(hash=TRUE, size=8*1024)

con <- file(args[1],open="r")
t1 <- 0.0
lc <- 0L
while(TRUE){
  line <- readLines(con,n=1)
  if (length(line) < 1) 
    break
  lc <- lc + 1L

  start <- proc.time()['elapsed']
  assign(line,"foo",envir=e);
  end <- proc.time()['elapsed']
  t1 <- t1 + end - start
}
close(con)
  

miss <- 0
t2 <- 0.0

con <- file(args[1],open="r")
while(TRUE){
  line <- readLines(con,n=1)
  if (length(line) < 1) 
    break

  start <- proc.time()['elapsed']
  m <- get(line,envir=e)
  end <- proc.time()['elapsed']
  t2 <- t2 + end - start

  # Miss should never miss!
  if (m!="foo") miss <<- miss + 1
}

cat(sprintf("%s\t%s\t%d\t%f\t%f\n",args[2],args[1],lc,t1,t2))
