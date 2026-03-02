Preparing Your Build Environment
================================

You have several options for creating a build environment in which you can build, debug, and run the applications within this repository:

1. [Native Linux Host](#native)
2. [Linux managed `chroot` environment](#schroot)
3. [Docker](#docker)


> **Note:** To run the inline commands below, open a shell prompt (e.g. a Linux terminal application) and navigate to your top-level work directory where you cloned this repository (or the parent repository if you are using this as a submodule).


## Native Linux Host{#native}

If, like me, you are running a recent Linux distribution (e.g., [Debian
12](https://www.debian.org/), [Ubuntu 24.04](https://ubuntu.com/download/desktop), or
newer) you can build directly on your host (or in a VM such as VirtualBox).  You will then need
to run the following command to install the required dependencies:

  ```bash
  $ make install-build-requirements
  ```

This will install all packages listed in the file [build/debian/build-requirements.txt](../../../build/debian/build-requirements.txt), using `sudo` to gain root access.  You will likely be prompted for your password.  If you do not yet have privileges, the simplest way to do so is to invoke the following command as the `root` user:

  ```bash
  # adduser USERNAME sudo
  ```

Replace `USERNAME` with your actual username.


## Docker{#docker}

### Preparation Steps

If you are using a different OS or an older version of Linux, the simplest way to create your development environment is to use the included Docker recipes.

#### Install Docker

There are essentially two options:

* [Docker Desktop](https://docs.docker.com/get-docker/) provides interactive control of your Docker instances via a tray in your system panel. You can launch it manually or set it to start whenever you log into your host system.

* [Docker Engine](https://docs.docker.com/engine/install/) runs a persistent, headless Docker daemon to supervise your instances. In this way your instances can continue to run even after you log out from your system.

  - If you are running on a Debian-based host system (incl. Ubuntu) you should then give yourself permission to use Docker by adding yourself to the `docker` group once installation has completed:

      ```bash
      sudo adduser $(whoami) docker
      ```

    Log out from your desktop environment and in again for this change to take effect.


#### Build Docker image

Run the following command:

  ```bash
  make docker_build
  ```

This will download the base OS image and install the required build components.


#### Start Docker container

This will launch the docker container in the background.

  ```bash
  make docker_up
  ```

> **Note**: Your home directory from the host system should now be mounted on `/home/${USERNAME}` within this container, where `${USERNAME}` is the passed in based on your own username in your host environment.


#### Enter a shell w/Docker container

Once a container is up and running you can use the following command to obtian access to a command shell on the inside (optionally from several terminals concurrently)

  ```bash
  make docker_shell
  ```

You are now ready to proceed to the next step, [Building](building.md).


### Shut down & Cleanup

Use the following commands to bring down and clean up any existing Docker instance

#### Stop Docker container

This will shut down the Docker instance, terminating any running processes including shells.

  ````bash
  make docker_down
  ````

#### Remove Docker container

This will remove the `common-core-build` docker image if it exists. Use this before building a new one to keep from building up stale images.

  ````bash
  make docker_clean
  ````


Next Steps
----------

Now that you have created your workspace and set up your build environment, proceed to

- [Obtaining the Source](cloning.md) for instructions on cloning this GIT repository into this environment if you haven't already done this, or

- [Building](building.md) for instructions on building the code in this repository.

