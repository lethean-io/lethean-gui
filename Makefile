DOCKER_IMAGE ?= lthn/gui
DOCKER_FILE ?= ./build-conf/linux.amd64.Dockerfile

.PHONY: docker
docker: build-docker
	docker run  --name="lthn-gui" -d "$(DOCKER_IMAGE)" && docker cp lthn-gui:/lethean-gui-linux-64bit-4.0.2.tar.bz2 ./dist/lethean-gui-linux-64bit-4.0.2.tar.bz2 && docker stop "lthn-gui" && echo "Deleting:" && docker container rm "lthn-gui"

# export the files to the current directory
.PHONY: build-docker
build-docker:
	docker build --no-cache -f "$(DOCKER_FILE)" -t "$(DOCKER_IMAGE)" ./gui
