VER=1.7.3

help:
	@echo "run on host"
	@echo "  docker-build-dev     - build kea-dev image"
	@echo "  docker-build         - build kea image"
	@echo "  docker-run-kea-dev   - run kea-dev shell"
	@echo "  docker-run-kea       - run kea shell"
	@echo "  docker-run-dhtest    - run dhtest shell"
	@echo "run inside kea-dev shell"
	@echo "  build-hook           - build and install libkea_python"
	@echo "  build-module         - build and install kea extension module"
	@echo "  test-module          - run unit tests for kea extension module"

docker-build-dev:
	docker build -f DockerfileDev --tag kea-dev:$(VER) .

docker-build:
	docker build --tag kea:$(VER) .

docker-run-kea-dev: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea-base kea-dev:$(VER) bash

docker-run-kea: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea kea:$(VER) bash

docker-run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/workdir dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea

build-hook:
	cd keahook && rm -f libkea_python.so && make install

build-module:
	cd keamodule && rm -rf build && python3 setup.py install

test-module:
	cd keamodule && nosetests3
