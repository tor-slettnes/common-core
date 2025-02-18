DOCKERFILE_DIR    ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
DOCKERFILE        ?= $(DOCKERFILE_DIR)/Dockerfile
CONTAINERNAME      = common-core-build
CONTAINERVERSION   = 1.0
CONTAINERTAG       = $(CONTAINERNAME):$(CONTAINERVERSION)
CONTAINER_USER     = $(shell id -un)
HOST_MOUNT         = $(HOME)
CONTAINER_MOUNT    = /home/$(CONTAINER_USER)
HOST_DIR          ?= $(shell realpath $(DOCKERFILE_DIR)/../..)
CONTAINER_DIR      = $(subst $(HOST_MOUNT),$(CONTAINER_MOUNT),$(HOST_DIR))

docker_build:
	@echo '+++ Building Docker image: $(CONTAINERTAG)'
	@docker build \
		--file $(DOCKERFILE) \
		--tag '$(CONTAINERTAG)' \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		--build-arg USERNAME=$(CONTAINER_USER) \
		.

docker_clean:
	@echo '--- Removing Docker image: $(CONTAINERTAG)'
	@docker image rm $(CONTAINERTAG)

docker_up:
	@echo '↑↑↑ Staring Docker instance, mounting $(HOST_MOUNT) on $(CONTAINER_MOUNT): $(CONTAINERTAG)'
	@docker run \
		--detach --rm --init \
		--name $(CONTAINERNAME) \
		--hostname $(CONTAINERNAME) \
		-v $(HOST_MOUNT):$(CONTAINER_MOUNT) \
		$(CONTAINERTAG)

docker_down:
	@echo -n '↓↓↓ Stopping Docker instance: '
	@docker stop $(CONTAINERNAME)

docker_shell:
	@echo '→→→ Entering Docker shell: $(CONTAINERNAME)'
	@docker exec --interactive --tty \
		--user "$(CONTAINER_USER)" \
		--workdir "$(CONTAINER_DIR)" \
		"$(CONTAINERNAME)" \
		/bin/bash || true
	@echo '←←← Exited Docker shell: $(CONTAINERNAME)'

.PHONY: docker_build docker_clean docker_up docker_down docker_shell
