.. _install:

------------------------------------------------------------------------------
Installation
------------------------------------------------------------------------------

libspatialindex supports building an installation through a `CMake`_-derived build system.

The CMake scenario for building libspatialindex is also quite simple. 

::

    [hobu@fire libspatialindex]$ cmake -DCMAKE_INSTALL_PREFIX=/home/marioh/usr . 

::

    [hobu@fire libspatialindex]$ make

::
    
    [hobu@fire libspatialindex]$ make install

.. _CMake: http://www.cmake.org
