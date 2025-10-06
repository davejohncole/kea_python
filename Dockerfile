ARG VER=latest
FROM kea-dev:${VER} AS build

WORKDIR /source
COPY . .

RUN make clean install \
    && mkdir /dist \
    && cd /usr/local \
    && find . -name \*.so\* | tar cf - -T - | (cd /dist; tar xf -) \
    && tar cf - etc share/man bin sbin var | (cd /dist; tar xf -)

FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y \
    && apt-get -y install \
        procps \
        socat \
        python3 \
        libpython3.12 \
        liblog4cplus-2.0.5t64 \
        libboost-system1.83.0 \
        libffi8ubuntu1 \
        libpq5 \
        libmariadb3

COPY --from=build /dist /usr/local

RUN ldconfig
