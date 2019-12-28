FROM debian:stretch-slim AS build

WORKDIR /source

RUN apt-get update -y \
    && apt-get -y install \
        wget \
        unzip \
        make \
        gcc \
    && wget https://github.com/saravana815/dhtest/archive/master.zip \
    && unzip master.zip \
    && cd dhtest-master \
    && make

FROM debian:stretch-slim

COPY --from=build /source/dhtest-master/dhtest /usr/local/bin
