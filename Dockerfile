ARG VER
FROM kea-dev:$VER AS build

WORKDIR /source
COPY . .

RUN make build-hook build-module \
    && mkdir /dist \
    && cd /usr/local \
    && find lib -name \*.so\* | tar cf - -T - | (cd /dist; tar xf -) \
    && tar cf - etc share/man bin sbin var | (cd /dist; tar xf -)

FROM debian:stretch-slim
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y \
    && apt-get -y install \
        procps \
        socat \
        python3 \
        libpython3.5 \
        liblog4cplus-1.1-9 \
        libboost-system1.62.0 \
        libffi6

COPY --from=build /dist /usr/local

RUN ldconfig
