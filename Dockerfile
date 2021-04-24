ARG VER
FROM kea-dev:$VER AS build

WORKDIR /source
COPY . .

RUN make clean install \
    && mkdir /dist \
    && cd /usr/local \
    && find lib -name \*.so\* | tar cf - -T - | (cd /dist; tar xf -) \
    && tar cf - etc share/man bin sbin var | (cd /dist; tar xf -)

FROM ubuntu:21.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y \
    && apt-get -y install \
        procps \
        socat \
        python3 \
        libpython3.9 \
        liblog4cplus-2.0.5 \
        libboost-system1.74.0 \
        libffi8ubuntu1 \
        libpq5 \
        libmariadb3

COPY --from=build /dist /usr/local

RUN ldconfig
