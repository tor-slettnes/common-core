DOCKERFILE_DIR    ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
DOCKERFILE        ?= $(DOCKERFILE_DIR)/Dockerfile
DOCKERBASE        ?= debian:bookworm
CONTAINERNAME      = picarro-shared
CONTAINERVERSION   = 1.1
CONTAINERTAG       = $(CONTAINERNAME):$(CONTAINERVERSION)
CONTAINER_USER     = $(shell id -un)
CONTAINER_UID      = $(shell id -u)
CONTAINER_GID      = $(shell id -g)
HOST_HOME          = $(HOME)
CONTAINER_HOME     = /home/$(CONTAINER_USER)
HOST_DIR           = $(shell realpath $(DOCKERFILE_DIR)/../..)
CONTAINER_DIR      = $(subst $(HOST_HOME),$(CONTAINER_HOME),$(HOST_DIR))

.PHONY: docker_build
docker_build:
	@if ! docker image inspect $(CONTAINERTAG) >/dev/null 2>&1; then \
		echo '+++ Building Docker image: $(CONTAINERTAG)'; \
		docker build \
			--file $(DOCKERFILE) \
			--tag '$(CONTAINERTAG)' \
			--build-arg DOCKERBASE=$(DOCKERBASE) \
			--build-arg USER=$(CONTAINER_USER) \
			--build-arg UID=$(CONTAINER_UID) \
			--build-arg GID=$(CONTAINER_GID) \
			.; \
	fi

.PHONY: docker_clean
docker_clean: docker_down
	@if docker image inspect $(CONTAINERTAG) >/dev/null 2>&1; then \
		echo '--- Removing Docker image: $(CONTAINERTAG)'; \
		docker image rm $(CONTAINERTAG); \
	fi

.PHONY: docker_up
docker_up: docker_build
	@if ! docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo '↑↑↑ Starting Docker container: $(CONTAINERTAG)'; \
		docker run \
			--detach --rm --init \
			--name $(CONTAINERNAME) \
			--hostname $(CONTAINERNAME) \
			$(foreach port,$(EXPOSE_TCP_PORTS),-p $(port):$(port)/tcp) \
			-v $(HOST_HOME):$(CONTAINER_HOME) \
			$(if $(WORKSPACE),-v $(WORKSPACE):$(WORKSPACE)) \
			$(if $(PYPIRC_FILE),-v $(PYPIRC_FILE):$(CONTAINER_HOME)/.pypirc:ro) \
			$(CONTAINERTAG); \
	fi

.PHONY: docker_down
docker_down:
	@if docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo -n '↓↓↓ Stopping Docker instance: '; \
		docker stop $(CONTAINERNAME); \
	fi

.PHONY: docker_shell
docker_shell: docker_up
	@echo '→→→ Entering interactive Docker shell: $(CONTAINERNAME)'
	@docker exec --interactive --tty \
		$(DOCKER_EXEC_OPTS) \
		--workdir "$(CONTAINER_DIR)" \
		"$(CONTAINERNAME)" \
		/bin/bash || true
	@echo '←←← Exited Docker shell: $(CONTAINERNAME)'

