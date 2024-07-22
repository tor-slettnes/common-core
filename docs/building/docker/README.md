Building with Docker
=========================

[Docker](https://docker.io/) provides customized virtual environments ("containers") across multiple OS platforms. It differs from full-fledged Virtual Machine software (such as [VirtualBox](https://virtualbox.org/), [VMWare](https://vmware.com) or [Parallels](https://parallels.com)) in several notable ways:

 * It's more lightweight, especially if your container and host operating systems are identical (e.g. Linux on Linux)
   - That said, it's still a virtual environment and thus more overhead than running inside a Linux `chroot` environment, using e.g. [schroot](https://wiki.debian.org/Schroot).

 * A large number of ready-to-use images are avaialble on [Docker Hub](https://hub.docker.com/). You can use these directly, or use them as basis for your own Docker containers.

 * It does _not_ emulate a complete running system.  For instance, a Docker container does not normally start system services (in fact, a Linux Docker container does not normally contain `systemd`).

In our case, Docker serves as a useful environment for building and running the code in this repository.


## Getting Docker

There are essentially two options:

* [Docker Desktop](https://docs.docker.com/get-docker/) provides interactive control of your Docker instances via a tray in your system panel. You can launch it manually or set it to start whenever you log into your host system.

* [Docker Engine](https://docs.docker.com/engine/install/) runs a persistent, headless Docker daemon to supervise your instances. In this way your instances can continue to run even after you log out from your system.

  - If you are running on a Debian-based host system (incl. Ubuntu) you should then give yourself permission to use Docker by adding yourself to the `docker` group once installation has completed:

      ```bash
      sudo adduser $(whoami) docker
      ```

    Log out from your desktop environment and in again for this change to take effect.


## Preparing a build environment with Docker

Follow these steps to create, launch, and use the Docker container `common-core-build`, which we can then use as a Linux-based build and test environment for our purposes.

### Create Docker container

From the `common-core` top-level folder, run the following command:

  ```bash
  make docker_build
  ```

### Start Docker container

This will launch the docker container in the background.

  ```bash
  make docker_up
  ```

**Note**: Your home directory from the host system is now mounted on `/home/${USERNAME}` within this container, where `${USERNAME}` is the passed in based on your own username in your host environment.


### Enter a shell w/Docker container

Once a container is up and running you can use the following command to obtian access to a command shell on the inside (optionally from several terminals concurrently)

  ```bash
  make docker_shell
  ```

You are now ready to proceed to the next step: [Building](../linux/building.md).



## Shut down & Cleanup

Use the following commands to bring down and clean up any existing Docker instance

### Stop Docker container

This will shut down the Docker instance, terminating any running processes including shells.

  ```bash
  make docker_down
  ```

### Remove Docker container

This will remove the `sam-shared-sdk` docker image if it exists. Use this before building a new one to keep from building up stale images.

  ```bash
  make docker_clean
  ```
