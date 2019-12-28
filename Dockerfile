FROM kea-base:1.7.3 AS build

WORKDIR /source
COPY . .

RUN cd keapyhook \
    && make \
    && cd ../keamodule \
    && python3 setup.py install

FROM debian:stretch-slim

RUN apt-get update -y \
    && apt-get -y install \
        procps \
        python3 \
        ldconfig

COPY --from=build /usr/local/lib /usr/local/lib
COPY --from=build /usr/local/lib/python3.5/dist-packages /usr/local/lib/python3.5/dist-packages
