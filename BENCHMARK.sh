#!/bin/bash -x

# Modify the below paths to R to test, one on each line
ALLR=(
  ~/Projects/R/R-Array-Hash/bin/R
  ~/Projects/R/optimized/devel/bin/R
)
DATASETS=(
  SKEW.1mil
  DISTINCT.500thou
  DISTINCT.1mil
)
#RUNS=`seq 1 10`
RUNS=1
#HASHSIZES=`Rscript -e 'cat(2^(10:19),"\n")'`
HASHSIZES=1024

OPTS='--vanilla --no-save'
RESULTS='results.csv'

rm -f $RESULTS error.log out.log

for run in $RUNS; do
for hashsize in $HASHSIZES; do
for dataset in ${DATASETS[*]}; do
for progname in ${ALLR[*]}; do
    $progname $OPTS --args $dataset $hashsize $run $RESULTS < RUN.R \
      1>>out.log 2>>error.log
    echo '-----------------------' >> error.log
done
done
done
done
