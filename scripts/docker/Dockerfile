FROM ubuntu:18.04
MAINTAINER Howard Butler <howard@hobu.co>

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        git libtool m4 automake

RUN git clone https://github.com/libspatialindex/libspatialindex.git


#RUN cd libspatialindex && \
#	cmake . -DCMAKE_INSTALL_PREFIX=/usr \
#		    -DCMAKE_SYSTEM_NAME=Linux \
#			-DCMAKE_BUILD_TYPE=Release \
#			&& make && make install
