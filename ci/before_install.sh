#!/bin/bash
# Installs requirements for libLAS
echo "$(tmstamp) *** before_install::apt-get starting $(date) ***"
sudo apt-get update -qq
sudo apt-get install -qq cmake
echo "$(tmstamp) *** before_install::apt-get finished $(date) ***"
gcc --version
clang --version
