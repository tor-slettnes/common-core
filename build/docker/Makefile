## -*- GNUMakefile -*-
#===============================================================================
## @file Makefile
## @brief Convenience targets for accessing Docker image via `docker-compose`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

export USER = $(shell id -un)
export UID  = $(shell id -u)
export GID  = $(shell id -g)

COMPOSER_NAME    ?= common-core
COMPOSER_SERVICE ?= runtime
DOCKER_IMAGE     ?= $(COMPOSER_NAME)-$(COMPOSER_SERVICE):latest
DOCKER_SHELL     ?= /bin/bash

HOST_HOME         = $(HOME)
HOST_DIR         ?= $(CURDIR)/../..

CONTAINER_HOME    = /home/$(USER)
CONTAINER_DIR     = $(subst $(HOST_HOME),$(CONTAINER_HOME),$(HOST_DIR))
DAEMON_DIR       ?= $(CONTAINER_DIR)/out/install/sbin

COMPOSE_COMMAND  ?= docker-compose
EXEC_COMMAND     ?= $(COMPOSE_COMMAND) exec -u $(USER) -w $(CONTAINER_DIR) \
                    $(COMPOSER_SERVICE)

.PHONY: docker_build
docker_build:
	@$(COMPOSE_COMMAND) build

.PHONY: docker_clean
docker_clean: docker_down
	@$(COMPOSE_COMMAND) rm -f -s
	@docker image rm -f $(DOCKER_IMAGE) >/dev/null 2>&1

.PHONY: docker_up
docker_up:
	@$(COMPOSE_COMMAND) up -d

.PHONY: docker_down
docker_down:
	@$(COMPOSE_COMMAND) stop

.PHONY: docker_shell
docker_shell: docker_up
	@$(EXEC_COMMAND) $(DOCKER_SHELL)

.PHONY: docker_exec_daemons
docker_exec_daemons: \
	docker_exec_daemon_demoserver \
	docker_exec_daemon_platformserver \
	docker_exec_daemon_multilogger \
	docker_exec_daemon_switchboard \

docker_exec_make_%: docker_up
	@echo '→→→ Building with Docker: $* $(MAKEOVERRIDES)'
	@$(EXEC_COMMAND) make $* $(MAKEOVERRIDES)

docker_exec_daemon_%: docker_exec_make_install
	@echo '→→→ Executing within Docker: $* --daemon'
	@$(EXEC_COMMAND) sudo $(DAEMON_DIR)/$* --daemon
