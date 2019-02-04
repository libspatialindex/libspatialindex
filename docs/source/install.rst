.. _install:

------------------------------------------------------------------------------
Installation
------------------------------------------------------------------------------

libspatialindex supports building and installation using a couple of 
different methods.  The primary method on unix-like systems is the 
`Autotools`_-style build.  Alternatively, a `CMake`_-derived build system is also
available for building libspatialindex as of the 1.7.0 release

Autotools
------------------------------------------------------------------------------

First run `autogen.sh` to generate the configure scripts.
By default include files and library files will be installed in `/usr/local`. If 
you would like to use a different installation directory (e.g., in case 
that you do not have root access) run the configure script with 
the --prefix option:

::

    [hobu@fire libspatialindex]$ ./configure --prefix=/home/marioh/usr

Make the library::

    [hobu@fire libspatialindex]$ make

Install the library::

    [hobu@fire libspatialindex]$ make install


CMake
------------------------------------------------------------------------------

The CMake scenario for building libspatialindex is also quite simple. 

::

    [hobu@fire libspatialindex]$ cmake -DCMAKE_INSTALL_PREFIX=/home/marioh/usr . 

::

    [hobu@fire libspatialindex]$ make

::
    
    [hobu@fire libspatialindex]$ make install

.. _CMake: http://www.cmake.org
.. _Autotools: http://en.wikipedia.org/wiki/GNU_build_system