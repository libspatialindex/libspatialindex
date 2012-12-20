#!/bin/sh
# $Id: autogen.sh 61 2007-12-11 20:13:48Z hobu $
#
# Autotools boostrapping script
#
giveup()
{
        echo
        echo "  Something went wrong, giving up!"
        echo
        exit 1
}

OSTYPE=`uname -s`

for libtoolize in glibtoolize libtoolize; do
    if which $libtoolize 1>/dev/null 2>/dev/null; then
        LIBTOOLIZE=`which $libtoolize 2>/dev/null`
        break;
    fi
done

if test "$LIBTOOLIZE" = ""; then
    echo
    echo "  glibtoolize or libtoolize not found. Giving up!"
    echo
    exit 1
fi

#AMFLAGS="--add-missing --copy --force-missing"
AMFLAGS="--add-missing --copy"
if test "$OSTYPE" = "IRIX" -o "$OSTYPE" = "IRIX64"; then
   AMFLAGS=$AMFLAGS" --include-deps";
fi

echo "Running aclocal"
aclocal || giveup
#echo "Running autoheader"
#autoheader || giveup
echo "Running libtoolize"
$LIBTOOLIZE --force --copy || giveup
echo "Running automake"
automake $AMFLAGS # || giveup
echo "Running autoconf"
autoconf || giveup

echo "======================================"
echo "Now you are ready to run './configure'"
echo "======================================"
