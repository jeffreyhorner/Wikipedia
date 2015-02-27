# If cinline is installed, then we'll use basic replacements for file(), readLines()
# and close() to eliminiate a little time to run the benchmark.
source('fileio.R')

# Size of largest significant cache: L1, L2, or L3? 
CACHE_SIZE <- 6 * 2^10

# Create and write to an array the size of the cache
flush_cache <- function(){
    x <- numeric(CACHE_SIZE)
    x[] <- rnorm(1)
    invisible() 
}

# Bogus value for each hash entry
val <- 'token'

args <- commandArgs(trailingOnly=TRUE)

if (length(args)!=4) stop("Need args: progname filename hashsize run")

progname = args[1]
FILENAME = args[2]
hashsize = as.integer(args[3])
run = as.integer(args[4])

if (!file.exists(FILENAME)) stop("No file!")

# The environment as hash to test
e <- new.env(hash=TRUE, size=hashsize)

constructHash <- function(filename, hashsize=2^10){
  con <- file(filename,open="r")
  on.exit(close(con))
  sum <- 0.0
  while(TRUE){
    line <- readLines(con,n=1)
    if (length(line) < 1) 
      break

    t1 <- proc.time()['elapsed']
    assign(line,val,envir=e);
    t2 <- proc.time()['elapsed']
    sum <- sum + (t2 - t1)
  }
  
  invisible(list(elapsed=sum, env=e))
}

miss <- 0

searchHash <- function(filename){
  con <- file(filename,open="r")
  on.exit(close(con))
  sum <- 0.0
  while(TRUE){
    line <- readLines(con,n=1)
    if (length(line) < 1) 
      break

    # Miss should never miss!
    t1 <- proc.time()['elapsed']
    if (get(line,envir=e)!='token') miss <<- miss + 1
    t2 <- proc.time()['elapsed']
    sum <- sum + (t2 - t1)
  }
 
  invisible(sum);
}

invisible(gcinfo(verbose=TRUE)) # For reporting number of gc's
flush_cache()
invisible(gc(reset=TRUE)) # Reset is important here to estimate
                          # the hash memory size.

# If R allows it, turn off gc altogether
if (any('on' %in% names(formals(gc))))
  invisible(gc(on=FALSE))

t1 <- proc.time()['elapsed']
x <- constructHash(FILENAME,hashsize)
t2 <- proc.time()['elapsed']
runsum <- t2 - t1;

z <- gc() # Estimation of hash memory size

flush_cache()
t1 <- proc.time()['elapsed']
y <- searchHash(FILENAME)
t2 <- proc.time()['elapsed']
runsum <- runsum + (t2 - t1)

write.table(
  data.frame(
    col1=rep(progname,8),
    col2=c("construction","search","memory","runtime","hashsize","miss","filename","run"),
    col3=c(x$elapsed, y, z[11] + z[12], runsum, hashsize, miss, FILENAME,run)
  ),
  file=stdout(),
  col.names=FALSE,
  row.names=FALSE
)
