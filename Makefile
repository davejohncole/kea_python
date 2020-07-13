ifeq "$(VER)" ""
	VER=1.6.1
endif

help:
	@echo "run on host"
	@echo "  build-kea-dev   - build kea-dev:$(VER) image"
	@echo "  build-kea       - build kea:$(VER) image"
	@echo "  build-dhtest    - build dhtest image"
	@echo "  run-kea-dev     - run kea-dev:$(VER) shell"
	@echo "  run-kea         - run kea:$(VER) shell"
	@echo "  run-mysql       - run mariadb for kea"
	@echo "  run-dhtest      - run dhtest shell"
	@echo "run inside kea-dev shell"
	@echo "  build-hook      - build and install libkea_python"
	@echo "  build-module    - build and install kea extension module"
	@echo "  test-module     - run unit tests for kea extension module"

build-kea-dev:
	docker build --build-arg VER=$(VER) -f DockerfileDev --tag kea-dev:$(VER) .

build-kea:
	docker build --build-arg VER=$(VER) --tag kea:$(VER) .

build-dhtest:
	cd dhtest && docker build --tag dhtest .

run-kea-dev: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea-dev kea-dev:$(VER) bash

run-kea: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir --name kea kea:$(VER) bash

run-mysql: kea-network
	docker run --rm --network kea -e MYSQL_ROOT_PASSWORD=admin -e MYSQL_DATABASE=kea -e MYSQL_USER=kea -eMYSQL_PASSWORD=kea --name mysql mariadb

run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/workdir --name dhtest dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea

build-hook:
	cd keahook && rm -f libkea_python.so && make install

build-module:
	cd keamodule && rm -rf build && python3 setup.py install

test-module:
	cd keamodule && nosetests3

.PHONY: help \
	build-kea-dev build-kea build-dhtest run-kea-dev run-kea run-mysql run-dhtest kea-network \
	build-hook build-module test-module
