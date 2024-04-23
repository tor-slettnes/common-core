CONTAINERNAME=common-core
CONTAINERVERSION=1.0
HOST_GIT=$(shell realpath ../..)
CONTAINER_GIT=/home/developer/$(CONTAINERNAME)

.PHONY: docker_build
docker_build:
	@echo "Building Docker Image: $(CONTAINERNAME)"
	docker build --tag $(CONTAINERNAME):$(CONTAINERVERSION) .

.PHONY: docker_up
docker_up:
	@echo "Starting Docker container: $(CONTAINERNAME)"
	docker run --name $(CONTAINERNAME) \
	--rm --interactive --tty \
	-v $(HOST_GIT):$(CONTAINER_GIT) \
	$(CONTAINERNAME):$(CONTAINERVERSION)

.PHONY: docker_down
docker_down:
	@echo "Stopping Docker container: $(CONTAINERNAME)"
	-docker stop $(CONTAINERNAME)

.PHONY: docker_shell
docker_shell:
	@echo "Opening debug shell to: $(CONTAINERNAME)"
	docker exec --interactive --tty $(CONTAINERNAME) /bin/bash
