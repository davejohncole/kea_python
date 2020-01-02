ifeq "$(VER)" ""
	VER=1.6.1
endif

help:
	@echo "run on host"
	@echo "  docker-build-dev     - build kea-dev:$(VER) image"
	@echo "  docker-build         - build kea:$(VER) image"
	@echo "  docker-build-dhtest  - build dhtest image"
	@echo "  docker-run-kea-dev   - run kea-dev:$(VER) shell"
	@echo "  docker-run-kea       - run kea:$(VER) shell"
	@echo "  docker-run-dhtest    - run dhtest shell"
	@echo "run inside kea-dev shell"
	@echo "  build-hook           - build and install libkea_python"
	@echo "  build-module         - build and install kea extension module"
	@echo "  test-module          - run unit tests for kea extension module"

docker-build-dev:
	docker build --build-arg VER=$(VER) -f DockerfileDev --tag kea-dev:$(VER) .

docker-build:
	docker build --build-arg VER=$(VER) --tag kea:$(VER) .

docker-build-dhtest:
	cd dhtest && docker build --tag dhtest .

docker-run-kea-dev: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea-dev kea-dev:$(VER) bash

docker-run-kea: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea kea:$(VER) bash

docker-run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/workdir --name dhtest dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea

build-hook:
	cd keahook && rm -f libkea_python.so && make install

build-module:
	cd keamodule && rm -rf build && python3 setup.py install

test-module:
	cd keamodule && nosetests3
