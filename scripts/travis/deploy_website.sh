#!/bin/bash

builddir=$1
destdir=$2
DATE=$(date +'%y.%m.%d %H:%M:%S')

git clone git@github.com:libspatialindex/libspatialindex.github.com.git $destdir/pdaldocs
cd $destdir/pdaldocs
git checkout master


cd $builddir/html
cp -rf * $destdir/pdaldocs

cd $builddir/latex/
cp libspatialindex.pdf $destdir/pdaldocs

cd $destdir/pdaldocs
git config user.email "pdal@hobu.net"
git config user.name "libspatialindex Travis docsbot"

git add -A
git commit -m "update with results of commit https://github.com/libspatialindex/libspatialindex/commit/$TRAVIS_COMMIT for ${DATE}"
git push origin master

