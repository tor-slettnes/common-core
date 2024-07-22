Preparing Your Build Environment
================================

The instructions below will help you set up your own Linux environment to develop, build, and test your code.

Alternatively, you can choose to build and run this code in a [Docker](https://www.docker.com/) environment on any supported platform. See [Building with Docker](../docker/README.md) for details. In this case the build environment is already included, and you may proceed to the [Building](building.md) step.


## Create a Linux environment

If you choose to roll your own Linux environment, you have a few options. We'll briefly mention some of them; please refer to their corresponding web sites for installation instructions.

### Host platform

You can create your Linux development environment

* natively on your computer, perhaps creating a separate disk partition so you can still keep your existing OS (you can then choose OS at boot time, or run one in a VM inside the other),

* entirely inside VM, such as [VirtualBox](https://www.virtualbox.org/), [VMWare](https://vmware.com), or [Parallels Desktop](https://parallels.com) (for Intel based Macs).

* using Windows Subsystem For Linux ([WSL](https://docs.microsoft.com/en-us/windows/wsl/)).

Which option you choose is a matter of getting started quickly vs. ongoing ease of use.  The VM route may be easiest to install, but with some performance penalty and ongoing complexity (especially if you also choose to share your working directory between VM and host in order to use native editors, GIT tools, etc).

### Distribution

The next step is to choose a Linux *distribution*.

* If you are new to Linux or if you like the idea of corporate backing, the latest [Ubuntu](https://ubuntu.com/download/desktop) will be a good choice. It has one of the simplest-to-use installers arund and a comprehensive out-of-box experience.  (*Note* that to build natively on Ubuntu, _the minimum requires version is 22.04!_ Some of the tools and libraries included in older versions are too far out of date for our needs already at the time of its release, for instance `python-3.11` and `cppzmq-dev`).

* If you don't need training wheels you wish to go straight to [Debian](https://www.debian.org/), the community-created, upstream base for Ubuntu (and ohers such as Mint). The installer asks a few more questions, but you will end up with in some ways a more flexible but simpler system with fewer dependency layers (for instance, system updates are provided directly via APT tools, `snapd` is an optional add-on and not a core requirement).

Please refer to the corresponding web sites to obtain installation instructions and media.


## Install required packages

Once you have your Debian or Ubuntu environment up, you will need to run the following commands to create your build environment:

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

* Google Testing framework (CMake option `BUILD_TESTING`):

  ```bash
  sudo apt install libgtest-dev
  ```

* ZeroMQ communication, including ZeroMQ with Protocol Buffer payloads (CMake option `BUILD_ZMQ`)

  ```bash
  sudo apt install cppzmq-dev protobuf-compiler
  ```

* gRPC communication (CMake option `BUILD_GRPC`)

  ```bash
  sudo apt install protobuf-compiler-grpc libgrpc++-dev
  ```

* HTTP requests, including REST API (CMake option `BUILD_HTTP`)

  ```bash
  sudo apt install libcurl4-gnutls-dev
  ```

* Network Manager integration over D-Bus (CMake option `BUILD_DBUS`):

  ```bash
  sudo apt install libglibmm-2.4-dev libnm-dev
  ```

* Linux UDev support for notifications related to removable drives (CMake optipon `BUILD_UDEV`):

  ```bash
  sudo apt install ilbudev-dev
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
  sudo apt install python3-protobuf python3-grpcio python3-zmq
  ```

### Dependencies used to generate developer/reference documentation:

   ```bash
   sudo apt install doxygen
   ```

### Third-party developer references:

   ```bash
   sudo apt install devhelp libglibmm-2.4-doc
   ```


Next Steps
----------

Now that you have created your workspace and set up your build environment, proceed to

- [Obtaining the Source](cloning.md) for instructions on cloning this GIT repository into this environment if you haven't already done this, or

- [Building](building.md) for instructions on building the code in this repository.

