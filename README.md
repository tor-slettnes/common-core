Getting Started
===============

Contents
--------

This repository contains source for the Common Core framework. Most of the code is developed in C++, but there is also client modules written in Python.

Directory Structure
-------------------

This repository is organized as follows:

* [cmake](cmake) -- CMake build recipes & include files
* [scripts](scripts) -- Miscellaneious scripts used for configuration, building & testing.
* [src](src) -- top-level source directory; see [source](src/README.md) for details.
  * [core](src/core) - Shared modules and libraries providing the application framework
  * [mantle](src/mantle) - Common applications/services, deployed across product lines
  * [crust](src/crust) - Glue code and configuration for specific products

In addition, an `out` directory is created as part of the build process:

* `out` - Artifacts generated as part of the build
  * `build` - Various outputs including generated code, object files, and executables.
  * `install` - Default installation folder
  * `docs` - Documentation generated from sources, readmes, etc. by Doxygen


Getting Started
---------------

## Preparing your development/build host

First, you need a Linux host computer to develop, build, and test your code.

* If you are new to Linux or if you like the idea of corporate backing, the latest [Ubuntu](https://ubuntu.com/download/desktop) will be a good choice. It has one of the simplest-to-use installers arund and a comprehensive out-of-box experience.  (*Note* that to build natively on Ubuntu, _the minimum requires version is 22.04!_ Some of the tools and libraries included in older versions are too far out of date for our needs already at the time of its release, for instance `python-3.11` and `cppzmq-dev`).

* If you don't need training wheels you wish to go straight to [Debian](https://www.debian.org/), the community-created, upstream base for Ubuntu (and ohers such as Mint). The installer asks a few more questions, but you will end up with in some ways a more flexible but simpler system with fewer dependency layers (for instance, system updates are provided directly via APT tools, `snapd` is an optional add-on and not a core requirement).

In either case, you may install this
* natively on your computer, perhaps creating a separate disk partition so you can still keep your existing OS (you can then choose OS at boot time, or run one in a VM inside the other),
* entirely inside VM, such as [VirtualBox](https://www.virtualbox.org/), or
* using Windows Subsystem For Linux ([WSL](https://docs.microsoft.com/en-us/windows/wsl/)).

Which option you choose is a matter of getting started quickly vs. ongoing ease of use.  The VM route may be easiest to install, but with some performance penalty and ongoing complexity (especially if you also choose to share your working directory between VM and host in order to use native editors, GIT tools, etc).

Lastly, if you have an older Linux system (for instance, Ubuntu 20.04 or older) that you cannot or will not upgrade, you can instead create a `schroot` container using the provided [create_schroot.sh](scripts/create_schroot.sh) script.  See details below.


## Obtaining the source

If you are reading this text directly on your system, you have already completed this step. Otherwise, you can obtain the source as follows (skip any steps you have already completed)

* Create a suitable GitHub Personal Access Token

  * Log into your GitHub account, and navigate to https://github.com/settings/tokens to generate your PAT. It's good idea to create specific tokens for each of your projects/employments, so that you can revoke it later if you wish.

* Set up GIT on your computer:
  * Open a Terminal window, and install the `git` command-line tool:

    ```bash
    $ sudo apt install git
    ```

  * Create or add to the file `$HOME/.gitconfig`:

    ```
    [user]
      name = Your Name
      email = you@email.com
    [credential]
      helper = store
    ```

    (Replace `Your Name` and `your.name` as appropriate).

  * Create or add to the file `$HOME/.git-credentials`:

    ```
    https://github-username:TOKEN@github.com
    ```

    (Replace `github-username` with your actual GitHub username, and `TOKEN` with a the contents of the Personal Access Token you created above).


* You should now be able to pull all required source code to build product from [`common-core`](https://github.com/torslettnes/common-core/) repository:

    ```bash
    $ git clone https://github.com/torslettnes/common-core.git
    ```


## Building and running wiithin a `schroot` environment

If you have an older Linux system and/or you want to use a separate build environment for this framework, you can do so using the provided [create_schroot.sh](scripts/create_schroot.sh) script:

  ```bash
  $ script/create_schroot.sh
  ```

This basically creates a `chroot` environment with automatic mounting/unmounting of various system folders including `/home` as you enter and exit.

Once this script completes, you can enter the `chroot` shell using

  ```bash
  $ schroot -c picarro-build
  ```

Or, because the script also creates a `default` alias, you can simply type

  ```bash
  $ schroot
  ```

Alternatively you can invoke the build directly:

  ```bash
  $ schroot make
  ```


## Compiling and running natively on your host system


### Install build dependencies (Debian/Ubuntu):

#### C++ build requirements:

* Core components:

  ```bash
  $ sudo apt install build-essential cmake pkgconf uuid-dev
  ```

* Optional components (use toggles in [CMakeLists.txt](CMakeLists.txt) to enable/disable):

  - Fast JSON parser for improved REST API performance (CMake option `BUILD_RAPIDJSON`)

    ```bash
    $ sudo apt install rapidjson-dev
    ```

  - Google Testing framework (CMake option `BUILD_TESTING`):

    ```bash
    $ sudo apt install libgtest-dev
    ```

  - ZeroMQ communication, including ZeroMQ with Protocol Buffer payloads (CMake option `BUILD_ZMQ`)

    ```bash
    $ sudo apt install cppzmq-dev protobuf-compiler
    ```

  - gRPC communication (CMake option `BUILD_GRPC`)

    ```bash
    $ sudo apt install protobuf-compiler-grpc libgrpc++-dev
    ```

    (This also installs ProtoBuf dependencies including `protobuf-compiler`.)

  - HTTP requests, including REST API (CMake option `BUILD_HTTP`)

    ```bash
    $ sudo apt install libcurl4-gnutls-dev
    ```

  - Python IPC (incl. pickling/unpickling) (CMake option `BUILD_PYTHON_IO`)

    ```bash
    $ sudo apt install python3-dev
    ```


#### Python runtime requirements


  ```bash
  $ sudo apt install python3-protobuf python3-grpcio python3-zmq
  ```

#### Dependencies used to generate developer/reference documentation:

   ```bash
   $ sudo apt install doxygen
   ```


### Build:

To start the build, simply invoke

  ```bash
  $ make
  ```

This will
  * run `cmake -B ${BUILD_DIR}`, where `${BUILD_DIR}` is `out/build/${TARGET}-${BUiLD_TYPE}` (e.g. `out/build/Linux-x86_64-Release`). This folder is created if needed.
  * run `cmake --build ${BUILD_DIR} --parallel $(nproc)` to perform the actual build
  * run `make -C "${BUILD_DIR}/src" test` to run available unit tests
  * run `cmake --install ${BUILD_DIR} --strip --prefix ${INSTALL_DIR}`, where ${INSTALL_DIR} is `out/install/${TARGET}`.  This will install the build outputs within your own working directory


If all went well the applications will now be installed within the the installation folder.
  * Server applications go under `sbin`
  * User-accessible applications go under `bin`
  * Python scripts go under `share/python`
  * Settings files go under `share/settings`.


### Installing in custom location:

You can optionally choose to install the applicaions in a different location;
for instance you may want to install into `/opt/common-core` if you want to
launch these applications as system services when your computer starts.  To do
so:

 * Ensure that you have write access to this folder:

   ```bash
   $ sudo mkdir -p /opt/common-core     # Create the folder if missing
   $ sudo chgrp staff /opt/common-core  # Set group ownership to `staff`
   $ sudo chmod g+ws /opt/common-core   # Set group write permission + sticky bits
   $ sudo adduser $(whoami) staff       # Add yourself to the `staff` group
   ```
   You may need to log out and back in again for this last change to take effect.


 * Speify the target folder as an argument to `make`:

  ```bash
  $ make INSTALL_DIR=/opt/common-core
  ```



## Writing code

To learn how to contribute, either by making updates to an existing application or by creating a new one, you may want to start with the [demo](src/mantle/apps/demo) application and its [documentation](src/mantle/apps/demo/README.md).



Questions? Comments? Concerns?
------------------------------

Please get in touch:

> Tor Slettnes  
> tor@slett.net  
> +1.925.8888.TOR
