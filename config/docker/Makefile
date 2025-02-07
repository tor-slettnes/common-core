DOCKERFILE_DIR    ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
DOCKERFILE        ?= $(DOCKERFILE_DIR)/Dockerfile
CONTAINERNAME      = common-core-build
CONTAINERVERSION   = 1.0
CONTAINERTAG       = $(CONTAINERNAME):$(CONTAINERVERSION)
HOST_MOUNT         = $(HOME)
CONTAINER_MOUNT    = /home/$(shell id -un)
HOST_DIR          ?= $(shell realpath $(DOCKERFILE_DIR)/../..)
CONTAINER_DIR      = $(subst $(HOST_MOUNT),$(CONTAINER_MOUNT),$(HOST_DIR))

docker_build:
	@echo 'Building Docker image: $(CONTAINERTAG)'
	docker build \
		--file $(DOCKERFILE) \
		--tag '$(CONTAINERTAG)' \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		--build-arg USERNAME=$(shell id -un) \
		.

docker_clean:
	@echo 'Removing Docker image: $(CONTAINERTAG)'
	docker image rm $(CONTAINERTAG)

docker_up:
	@echo 'Staring Docker instance, mounting $(HOST_MOUNT) on $(CONTAINER_MOUNT): $(CONTAINERTAG)'
	docker run \
		--detach --rm --init \
		--name $(CONTAINERNAME) \
		--hostname $(CONTAINERNAME) \
		-v $(HOST_MOUNT):$(CONTAINER_MOUNT) \
		$(CONTAINERTAG)

docker_down:
	@echo 'Stopping Docker instance: $(CONTAINERNAME)'
	docker stop $(CONTAINERNAME)

docker_shell:
	@echo 'Opening Docker shell: $(CONTAINERNAME)'
	docker exec -it -w $(CONTAINER_DIR) $(CONTAINERNAME) /bin/bash

.PHONY: docker_build docker_up docker_down docker_shell
