#!/bin/bash

TRAVIS_BUILD_DIR=$1
echo "building docs for $TRAVIS_BUILD_DIR/docs"
docker run -v $TRAVIS_BUILD_DIR:/data -w /data/docs osgeo/proj-docs make doxygen
docker run -v $TRAVIS_BUILD_DIR:/data -w /data/docs osgeo/proj-docs make html
docker run -v $TRAVIS_BUILD_DIR:/data -w /data/docs osgeo/proj-docs make latexpdf


