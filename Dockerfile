FROM debian:stretch-slim

WORKDIR /source
COPY kea-1.7.3.tar.gz .

RUN apt-get update -y \
    && apt-get -y install \
        procps \
        make \
        g++ \
        libboost-dev \
        libboost-system-dev \
        libssl-dev \
        liblog4cplus-dev \
        libpython3-dev \
        python3 \
        python3-nose \
    && tar xzf kea-1.7.3.tar.gz \
    && cd kea-1.7.3 \
    && ./configure --enable-shell --with-site-packages=/usr/lib/python3/dist-packages --enable-perfdhcp --enable-generate-messages \
    && make \
    && make install \
    && ldconfig
