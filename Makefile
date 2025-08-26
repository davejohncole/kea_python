ifeq "$(VER)" ""
	VER=3.0.0
endif

help:
	@echo "run on host"
	@echo "  build-kea-dev   - build kea-dev:$(VER) image"
	@echo "  build-kea       - build kea:$(VER) image"
	@echo "  build-dhtest    - build dhtest image"
	@echo "  run-kea-dev     - run kea-dev:$(VER) shell"
	@echo "  run-kea         - run kea:$(VER) shell"
	@echo "  run-mysql       - run mariadb for kea with schema for $(VER)"
	@echo "  run-dhtest      - run dhtest shell"
	@echo "run on host or inside kea-dev shell"
	@echo "  build           - build-hook and build-module"
	@echo "  install         - install-hook and install-module"
	@echo "  clean           - remove all generated files"
	@echo "  build-hook      - build libkea_python"
	@echo "  build-module    - build kea extension module"
	@echo "  clean-module    - delete kea extension module build files"
	@echo "  install-hook    - install libkea_python"
	@echo "  install-module  - install kea extension module"
	@echo "  test-module     - run unit tests for kea extension module"

build-kea-dev:
	docker build --build-arg VER=$(VER) -f DockerfileDev --tag kea-dev:$(VER) .

build-kea:
	docker build --build-arg VER=$(VER) --tag kea:$(VER) .

build-dhtest:
	cd dhtest && docker build --tag dhtest .

run-kea-dev: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir -w /workdir --name kea-dev --hostname kea kea-dev:$(VER) bash

run-kea: kea-network
	docker run --rm -it --network kea -e LANG=C.UTF-8 --privileged -v`pwd`:/workdir -w /workdir --name kea --hostname kea kea:$(VER) bash

run-mysql: kea-network dhcpdb_create.mysql.sql
	docker run --rm --network kea \
		-e MYSQL_ROOT_PASSWORD=admin -e MYSQL_DATABASE=kea -e MYSQL_USER=kea -eMYSQL_PASSWORD=kea \
		--name mysql --hostname mysql \
		-v `pwd`/dhcpdb_create-$(VER).mysql.sql:/docker-entrypoint-initdb.d/dhcpdb_create.mysql.sql \
		mariadb --general_log

run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/workdir --name dhtest dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea

build: build-hook build-module

build-hook: settings.mk
	cd keahook && make libkea_python.so

build-module: settings.mk
	cd keamodule && python3 setup.py build

clean-module:
	cd keamodule && rm -rf build

install: install-hook install-module

install-hook: build-hook
	cd keahook && make install

install-module: build-module
	cd keamodule && python3 setup.py install --single-version-externally-managed --root=/

clean:
	touch settings.mk
	cd keahook && make clean
	cd keamodule && rm -rf build
	rm -f settings.mk dhcpdb_create.mysql.sql

settings.mk:
	python3 settings.py

dhcpdb_create.mysql.sql:
	tar x --strip-components 6 -f kea-$(VER).tar.* kea-$(VER)/src/share/database/scripts/mysql/dhcpdb_create.mysql
	mv dhcpdb_create.mysql dhcpdb_create-$(VER).mysql.sql

test-module:
	PYTHONPATH=$(wildcard keamodule/build/lib.*) python3 -m unittest discover -s keamodule/tests

.PHONY: help \
	build-kea-dev build-kea build-dhtest run-kea-dev run-kea run-mysql run-dhtest kea-network \
	build-hook build-module test-module
