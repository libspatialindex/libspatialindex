export CONDA_EXE=/Users/hobu/miniconda3/bin/conda
source /Users/hobu/miniconda3/bin/activate base


$CONDA_EXE remove libspatialindex
$CONDA_EXE activate libspatialindex
source /Users/hobu/miniconda3/bin/activate libspatialindex
$CONDA_EXE create --name libspatialindex -y -c conda-forge \
    cmake \
    ninja

BUILDDIR=conda-build

#CONFIG="Unix Makefiles"
CONFIG="Ninja"

if ! [ -z "$1" ]; then
    CONFIG="$1"
fi


rm -rf $BUILDDIR
mkdir -p $BUILDDIR
cd $BUILDDIR
CC=$CC CXX=$CXX cmake   -G "$CONFIG"  \
        -DCMAKE_LIBRARY_PATH:FILEPATH="$CONDA_PREFIX/lib" \
        -DCMAKE_INCLUDE_PATH:FILEPATH="$CONDA_PREFIX/include" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
        ..



if ! [ -z "Ninja" ]; then
    ninja
fi

