#!/bin/bash


# Build PDAL package
# ./package.sh


GITSHA="$(git rev-parse HEAD)"
VERSIONTAG="$(git describe --exact-match --tags $(git log -n1 --pretty='%h'))"

echo "Cutting release for SHA $GITSHA at tag $VERSIONTAG"

HERE=`pwd`
CONTAINER="osgeo/proj-docs"
DOCKER="docker"
CONTAINERRUN="$DOCKER run -it -d --entrypoint /bin/sh -v $HERE:/data $CONTAINER"
echo $CONTAINERRUN


CONTAINERID=`$CONTAINERRUN`
echo "Starting container: " $CONTAINERID
cat > docker-package.sh << "EOF"
#!/bin/sh

git clone https://github.com/libspatialindex/libspatialindex.git;
cd /libspatialindex;
EOF

echo "git checkout $GITSHA" >> docker-package.sh

cat >> docker-package.sh << "EOF"
apt-get install cmake -y
mkdir build; cd build
cmake ..
make dist
EOF

echo "OUTPUTDIR=\"/data/release-$VERSIONTAG\"" >> docker-package.sh

cat >> docker-package.sh << "EOF"
mkdir $OUTPUTDIR

extensions=".tar.gz .tar.bz2"
for ext in $extensions
do


    for filename in $(ls *$ext)
    do

        `md5sum $filename > $filename.md5`
        `sha256sum $filename > $filename.sha256sum`
        `sha512sum $filename > $filename.sha512sum`
        cp $filename $OUTPUTDIR
        cp $filename.md5 $OUTPUTDIR
        cp $filename.sha256sum $OUTPUTDIR
        cp $filename.sha512sum $OUTPUTDIR
    done
done


EOF

chmod +x docker-package.sh
docker cp docker-package.sh $CONTAINERID:/docker-package.sh

docker exec -it $CONTAINERID /docker-package.sh

# run this to halt into the container
#docker exec -it $CONTAINERID bash

command="$DOCKER stop $CONTAINERID"
echo $command
$command

