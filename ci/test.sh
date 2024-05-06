#!/bin/bash

ls `pwd`
env

ctest -VV --rerun-failed --output-on-failure
