VER=1.7.3

help:
	@echo "run on host"
	@echo "  docker-build-base    - build kea-base image"
	@echo "  docker-build         - build kea image"
	@echo "  docker-run-kea-base  - run kea-base shell"
	@echo "  docker-run-dhtest    - run dhtest shell"
	@echo "run inside kea-base shell"
	@echo "  build-hook           - build and install libdhcp_python"
	@echo "  build-module         - build and install kea extension module"
	@echo "  test-module          - run unit tests for kea extension module"

docker-build-base:
	docker build -f DockerfileBase --tag kea-base:$(VER) .

docker-build:
	docker build --tag kea:$(VER)

docker-run-kea-base: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/keapy --name kea-base kea-base:1.7.3 bash

docker-run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/keapy dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea

build-hook:
	cd dhcp_python && rm -f libdhcp_python.so && make install

build-module:
	cd keamodule && rm -rf build && python3 setup.py install

test-module:
	cd keamodule && nosetests3
