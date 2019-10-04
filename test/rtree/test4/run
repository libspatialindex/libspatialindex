#! /bin/bash

echo Generating dataset
test-rtree-Generator 10000 0 > d
awk '{if ($1 == 1) print $0}' < d > data
awk '{if ($1 == 2) print $0}' < d > queries
rm -rf d

echo Creating new R-Tree
test-rtree-RTreeLoad data tree 20 selfjoin

echo Querying R-Tree
test-rtree-RTreeQuery queries tree selfjoin > res
cat data queries > .t

echo Running exhaustive search
test-rtree-Exhaustive .t selfjoin > res2

echo Comparing results
sort -n res > a
sort -n res2 > b
if diff a b
then
echo "Same results with exhaustive search. Everything seems fine."
echo Results: `wc -l a`
rm -rf a b res res2 .t tree.*
else
echo "PROBLEM! We got different results from exhaustive search!"
fi
