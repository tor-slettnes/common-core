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
	@if ! docker image inspect $(CONTAINERTAG) >/dev/null 2>&1; then \
		echo '+++ Building Docker image: $(CONTAINERTAG)'; \
		docker build \
			--file $(DOCKERFILE) \
			--tag '$(CONTAINERTAG)' \
			--build-arg DOCKERBASE=$(DOCKERBASE) \
			--build-arg UID=$(shell id -u) \
			--build-arg GID=$(shell id -g) \
			--build-arg USERNAME=$(shell id -un) \
			.; \
	fi

docker_clean: docker_down
	@if docker image inspect $(CONTAINERTAG) >/dev/null 2>&1; then \
		echo '--- Removing Docker image: $(CONTAINERTAG)'; \
		docker image rm $(CONTAINERTAG); \
	fi

docker_up: docker_build
	@if ! docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo '↑↑↑ Starting Docker container: $(CONTAINERTAG)'; \
		docker run \
			--detach --rm --init \
			--name $(CONTAINERNAME) \
			--hostname $(CONTAINERNAME) \
			-v $(HOST_MOUNT):$(CONTAINER_MOUNT) \
			$(if $(WORKSPACE),-v $(WORKSPACE):$(WORKSPACE)) \
			$(CONTAINERTAG); \
	fi

docker_down:
	@if docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo -n '↓↓↓ Stopping Docker instance: '; \
		docker stop $(CONTAINERNAME); \
	fi

docker_shell: docker_up
	@echo '→→→ Entering Docker shell: $(CONTAINERNAME)'
	@docker exec --interactive --tty \
		$(DOCKER_EXEC_OPTS) \
		--user "$(CONTAINER_USER)" \
		--workdir "$(CONTAINER_DIR)" \
		"$(CONTAINERNAME)" \
		/bin/bash || true
	@echo '←←← Exited Docker shell: $(CONTAINERNAME)'

.PHONY: docker_build docker_clean docker_up docker_down docker_shell
