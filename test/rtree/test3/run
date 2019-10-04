#! /bin/bash


#echo Generating 10 million entries. This might take a while
#echo Generating dataset
test-rtree-Generator 1000000 0 > d
awk '{if ($1 == 1) print $0}' < d > data
awk '{if ($1 == 2) print $0}' < d > queries
rm -rf d

echo Creating new R-Tree
test-rtree-RTreeBulkLoad data tree 1000 0.9

echo Querying R-Tree
test-rtree-RTreeQuery queries tree intersection > res
cat data queries > .t

echo Running exhaustive search
test-rtree-Exhaustive .t intersection > res2

echo Comparing results
sort -n res > a
sort -n res2 > b
if diff a b
then echo "Same results with exhaustive search. Everything seems fine."
else echo "PROBLEM! We got different results from exhaustive search!"
fi
echo Results: `wc -l a`
rm -rf a b res res2 .t tree.*
