#!/bin/bash -x

# Modify the below paths to R to test, one on each line
ALLR=(
  ~/Projects/R/R-Array-Hash/bin/Rscript
  ~/Projects/R/optimized/R-3.2.2/bin/Rscript
)
#DATASETS=(
#  DISTINCT.1mil
#)
DATASETS=(
  SKEW.1mil
  SKEW.2mil
  SKEW.3mil
  SKEW.4mil
  SKEW.5mil
  SKEW.6mil
  SKEW.7mil
  SKEW.8mil
  DISTINCT.1mil
  DISTINCT.2mil
  DISTINCT.3mil
  DISTINCT.4mil
  DISTINCT.5mil
  DISTINCT.6mil
  DISTINCT.7mil
  DISTINCT.8mil
)
RUNS=`seq 1 3`
OPTS='--vanilla --quiet'
RESULTS='results.csv'

rm -f $RESULTS error.log

for run in $RUNS; do
#./construct_search $dataset >>$RESULTS
for progname in ${ALLR[*]}; do
for dataset in ${DATASETS[*]}; do
    $progname $OPTS construct_search.R $dataset $progname \
      1>>$RESULTS 2>>error.log
    echo '-----------------------' >> error.log
done
#./construct_search.py $dataset >>$RESULTS
done
done
