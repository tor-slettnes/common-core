DOCKERFILE_DIR    ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
DOCKERFILE        ?= $(DOCKERFILE_DIR)/Dockerfile
DOCKERBASE        ?= debian:bookworm
CONTAINERNAME      = common-core
CONTAINERVERSION   = 1.0
CONTAINERTAG       = $(CONTAINERNAME):$(CONTAINERVERSION)
CONTAINER_USER     = $(shell id -un)
HOST_MOUNT         = $(HOME)
CONTAINER_MOUNT    = /home/$(CONTAINER_USER)
HOST_DIR          ?= $(shell realpath $(DOCKERFILE_DIR)/../..)
CONTAINER_DIR      = $(subst $(HOST_MOUNT),$(CONTAINER_MOUNT),$(HOST_DIR))
OUT_DIR            ?= out/docker
INSTALL_DIR        ?= $(OUT_DIR)/install
DAEMON_DIR         ?= $(CONTAINER_DIR)/$(INSTALL_DIR)/sbin
CONTAINER_LOG_DIR  ?= /var/log/common-core
HOST_LOG_DIR       ?= $(HOST_DIR)/$(OUT_DIR)/logs

.PHONY: docker_build
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
			--build-arg LOG_DIR=$(CONTAINER_LOG_DIR) \
			.; \
	fi

.PHONY: docker_clean
docker_clean: docker_down
	@if docker image inspect $(CONTAINERTAG) >/dev/null 2>&1; then \
		echo '--- Removing Docker image: $(CONTAINERTAG)'; \
		docker image rm $(CONTAINERTAG); \
	fi

.PHONY: docker_up
docker_up: docker_build $(HOST_LOG_DIR)
	@if ! docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo '↑↑↑ Starting Docker container: $(CONTAINERTAG)'; \
		docker run \
			--detach --rm --init \
			--name $(CONTAINERNAME) \
			--hostname $(CONTAINERNAME) \
			--publish-all \
			-v $(HOST_MOUNT):$(CONTAINER_MOUNT) \
			$(if $(WORKSPACE),-v $(WORKSPACE):$(WORKSPACE)) \
			$(if $(PYPIRC_FILE),-v $(PYPIRC_FILE):$(CONTAINER_MOUNT)/.pypirc:ro) \
			$(if $(CONTAINER_LOG_DIR),-v $(HOST_LOG_DIR):$(CONTAINER_LOG_DIR)) \
			$(CONTAINERTAG); \
	fi

.PHONY: docker_down
docker_down:
	@if docker container inspect $(CONTAINERNAME) >/dev/null 2>&1; then \
		echo -n '↓↓↓ Stopping Docker instance: '; \
		docker stop $(CONTAINERNAME); \
	fi

.PHONY: docker_exec_daemons
docker_exec_daemons: \
	docker_exec_daemon_multilogger \
	docker_exec_daemon_switchboard \
	docker_exec_daemon_platformserver

docker_exec_make_%: docker_up
	@echo '→→→ Building with Docker: $* $(MAKEOVERRIDES)'
	@docker exec \
		--user "$(CONTAINER_USER)" \
		--workdir "$(CONTAINER_DIR)" \
		--env OUT_DIR=$(CONTAINER_DIR)/$(OUT_DIR) \
		"$(CONTAINERNAME)" \
		make $* $(MAKEOVERRIDES)

docker_exec_daemon_%: docker_exec_make_install
	@echo '→→→ Executing within Docker: $* --daemon'
	@docker exec \
		"$(CONTAINERNAME)" \
		$(DAEMON_DIR)/$* --daemon

docker_shell: docker_up
	@echo '→→→ Entering interactive Docker shell: $(CONTAINERNAME)'
	@docker exec --interactive --tty \
		$(DOCKER_EXEC_OPTS) \
		--user "$(CONTAINER_USER)" \
		--workdir "$(CONTAINER_DIR)" \
		--env OUT_DIR=$(CONTAINER_DIR)/$(OUT_DIR) \
		"$(CONTAINERNAME)" \
		/bin/bash || true
	@echo '←←← Exited Docker shell: $(CONTAINERNAME)'

$(HOST_LOG_DIR):
	@mkdir -p "$@"
