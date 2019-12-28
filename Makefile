VER=1.7.3

help:
	@echo "build-base    - build kea-base:$(VER) image"
	@echo "build         - build kea:$(VER) image"
	@echo "run-kea-base  - run kea-base:$(VER) shell"
	@echo "run-dhtest    - run dhtest shell"

build-base:
	docker build -f DockerfileBase --tag kea-base:$(VER) .

build:
	docker build --tag kea:$(VER)

run-kea-base: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/keapy --name kea-base kea-base:1.7.3 bash

run-dhtest: kea-network
	docker run --rm -it --network kea --privileged -v`pwd`:/keapy dhtest bash

kea-network:
	docker network ls | grep -q kea || docker network create --subnet=172.28.5.0/24 --ip-range=172.28.5.0/24 kea
