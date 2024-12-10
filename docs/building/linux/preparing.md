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
to run the following commands to install the required dependencies.


### C++ build requirements:

* Common build tools

  ```bash
  sudo apt install build-essential cmake pkgconf
  ```

* UUID generator library

  ```bash
  sudo apt install uuid-dev
  ```

* Fast JSON parser for improved REST API performance (CMake option `BUILD_RAPIDJSON`)

  ```bash
  sudo apt install rapidjson-dev
  ```

* Yaml parser (CMake option `BUILD_YAML`)

  ```bash
  sudo apt install rapidjson-dev
  ```

* SQLite3 database (e.g. for logging) (CMake option `BUILD_SQLITE3`)

  ```bash
  sudo apt install rapidjson-dev
  ```

* Avahi mDNS library (CMake option `BUILD_DNSSD_AVAHI`)

  ```bash
  sudo apt install rapidjson-dev
  ```

* Google Testing framework (CMake option `BUILD_TESTING`):

  ```bash
  sudo apt install libgtest-dev
  ```

* HTTP requests, including REST API (CMake option `BUILD_HTTP`)

  ```bash
  sudo apt install libcurl4-gnutls-dev
  ```

* ProtoBuf message format (CMake option `BUILD_PROTOBUF`)

  ```bash
  sudo apt install protobuf-compiler
  ```

* ZeroMQ communication, including ZeroMQ with Protocol Buffer payloads (CMake option `BUILD_ZMQ`)

  ```bash
  sudo apt install cppzmq-dev
  ```

* gRPC communication (CMake option `BUILD_GRPC`)

  ```bash
  sudo apt install protobuf-compiler-grpc libgrpc++-dev
  ```

* Kafka communication (CMake options `BUILD_KAFKA`, `BUILD_AVRO`)

  ```bash
  sudo apt install librdkafka-dev libavro-dev
  ```

* Network Manager integration over D-Bus (CMake option `BUILD_DBUS`):

  ```bash
  sudo apt install libglibmm-2.4-dev libnm-dev
  ```

* Linux UDev support for notifications related to removable drives (CMake optipon `BUILD_UDEV`):

  ```bash
  sudo apt install libudev-dev
  ```

* Python IPC (incl. pickling/unpickling) (CMake option `BUILD_PYTHON_IO`)

  ```bash
  sudo apt install python3-dev
  ```


### Python package (wheel) creation

  ```shell
  sudo apt install python3-build python3-venv python3-hatchling
  ```

### Python runtime requirements

  ```bash
  sudo apt install python3-protobuf python3-grpcio python3-zmq python3-kafka python3-avro
  ```

### Dependencies used to generate developer/reference documentation:

   ```bash
   sudo apt install doxygen
   ```

### Third-party developer references:

   ```bash
   sudo apt install devhelp libglibmm-2.4-doc cppreference-doc-en-html
   ```

## Linux managed `chroot` environment{#schroot}

If you have an older Linux system and/or you want to use a separate build environment for this software, you can perform the build inside a managed `chroot` environment, using the tool [schroot](https://wiki.debian.org/Schroot).

You can think of `schroot` as a lightweight alternative to virtualization environments such as Docker, except that there are no machine instances to start or stop - simply enter and exit the environment at will.  User names, home directories, etc are explicitly shared with the outside host, which also means it's more seamlessly integrated into your workspace compared to Docker.

To set up such a `schroot` build environment from an existing Debian or Ubuntu system you can use the provided [scripts/create_schroot.sh](../../scripts/create_schroot.sh) script:

  ```bash
  scripts/create_schroot.sh
  ```

Once this script completes, you can enter the `chroot` shell using

  ```bash
  schroot -c common-core
  ```

Or, because the script also creates a `default` alias, you can simply type

  ```bash
  schroot
  ```

If you do this from somewhere within your home directory, you'll be placed in the same working directory within the `chroot` environment.

Alternatively you can invoke commands within this `chroot` environment directly from your host by prefixing them with `schroot`:

  ```bash
  schroot make
  ```


## Docker{#docker}

Last but not least, you can use [Docker Desktop](https://docs.docker.com/desktop/) or [Docker Engine](https://docs.docker.com/engine/) to build and run the software regardless of your host OS. While not as lightweight or as tightly integrated into your host environment as the `schroot` option above, this does at least allow you to share your working directory between your build environment and the host.

### Preparation Steps

To prepare your Docker environment, follow these steps.

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

