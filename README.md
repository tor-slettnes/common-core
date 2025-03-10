Getting Started
===============

Contents
--------

This repository contains source for the Common Core framework. Most of the code is developed in C++, but there is also client modules written in Python.

Directory Structure
-------------------

This repository is organized as follows:

* [docs](docs/) -- General documentation including getting started guides
* [build](build/) -- Build settings, recipes, environments
  * [cmake](build/cmake/) -- CMake build recipes & include files
  * [docker](build/docker/) -- Recipes to make Docker image
  * [scripts](build/scripts/) -- Miscellaneious scripts used for configuration, building & testing.
* [src](src/) -- top-level source directory; see [source](src/README.md) for details.
  * [core](src/core) - Shared modules and libraries providing the application framework
  * [mantle](src/mantle) - Common applications/services, deployed across product lines

In addition, an `out` directory is created as part of the build process:

* `out` - Artifacts generated as part of the build
  * `build` - Various outputs including generated code, object files, and executables.
  * `install` - Default installation folder
  * `packages` - Deployable installation packages (currently Debian packages and Python wheels)
  * `docs` - Documentation generated from sources, readmes, etc. by Doxygen


Getting Started
---------------

To build, install, and run the applications in this repository, follow these steps:

* [Getting Started on Linux](docs/building/linux/README.md)


### Docker

In particular, if your computer is running a non-Linux OS and you have installed [Docker Desktop](https://docs.docker.com/desktop/) or [Docker Engine](https://docs.docker.com/engine/), the following should get you started:

  * Build a Docker image:

    ```shell
    make docker_build
    ```

  * Start a Docker container from this image in the background:

    ```shell
    make docker_up
    ```

  * Launch a command shell within this Docker container:

    ```shell
    make docker_shell
    ```


## Building, Installing, and Invoking

Once you have completed the steps above, you are ready to build.

### Building and running in place

Within your workspace (which may be inside a virtual environment as per above), issue the following command:

  ```shell
  make
  ```

Without any arguments this builds the default/first [Makefile](Makefile) target, `local`, and is therefore equivalent to

  ```shell
  make local
  ```

This will build and install runtime components under `out/install`:
  * Server executables such as `platformserver` go under `out/install/sbin`
  * Client utilities such as `netconfig-tool` and `platform-shell` go under `out/install/bin`
  * Python modules go under `out/install/lib/python3/dist-packages/cc`
  * Default settings go under `out/install/share/common-core/settings`.


### Additional `Makefile` targets

Additional targets are defined
 * in [Makefile](Makefile/), and
 * by the various CMake targets defined throughout the codebase.

To get a list of available targets, invoke

  ```shell
  make list
  ```

### Building a Debian package

Once you are ready to release the software, you can create a Debian/Ubuntu installation package (`.deb`) by invoking:

  ```shell
  make deb
  ```

This will produce one or more `.deb` files under `out/packages/`. To make a specific version, use:

  ```shell
  make deb VERSION=x.y.z
  ```

### Installing a Debian package

To install this on your Debian-based system, use this (again, replacing `VERSION` with the actual package version):

  ```shell
  sudo apt -y install ./out/packages/cc-*-VERSION.deb
  ```

The `sudo` command is used to gain `root` privileges in order to run the `apt` utility.

(Note that the leading `./` is required for the `apt` tool to recognize this as a local filename, rather than a package name to be retrieved from your Debian/Ubuntu repository).


### Launching installed applications

Once this has completed, you will find runtime components in standard system folders under `/usr`.

#### Launching server applications:

Server applications will normally be started automatically by `systemd`. If you prefer to run it interactively, for instance to see log messages printed to your terminal, first use `systemctl` to stop/disable the service.

For instance, to run `demoserver` interactively:

  ```shell
  sudo systemctl stop demoserver      # Stop the service
  sudo systemctl disable demoserver   # Keep it from automatically launching at boot
  /usr/sbin/demoserver                # Launch `platformserver` in the foreground
  ```

If you later wish to re-enable the system service, use the following:

  ```shell
  sudo systemctl enable demoserver
  sudo systemctl start demoserver
  ```

### Launching client utilities

Command-line tools and utilities are typically installed under `/usr/bin`, and can be invoked directly.  For instance:

  ```shell
  demo-grpc-tool get_current_time
  ```

or

  ```shell
  demoshell
  ```


## Tweaking your build options

CMake is configured (via [CMakeLists.txt](CMakeLists.txt)) to load various build settings from `build/local.cmake` as well as [build/defaults.cmake](build/defaults.cmake); however only the latter is present in this repository.

To customize your build options without accidentally committing your changes, you can create your own `build/local.cmake` file; this is listed in [build/.gitignore](build/.gitignore) and therefore will not be committed to Git.  For example:

  ```cmake
   # local.cmake: Local overrides to settings from `defaults.cmake`

   set(PRODUCT "myproduct")
   set(UPGRADE_SCAN_URL "https://my.site/package-index.json")
   set(BUILD_DEMO OFF)
   ```

### Priority order

Settings are loaded from these files in order:

 1. [build/buildspec.cmake](build/buildspec.cmake), which in turn loads
      a. `build/local.cmake`, if present
      b. [build/defaults.cmake](build/defaults.cmake)

By convention, the variables listed in `defaults.cmake` are CMake [options](https://cmake.org/cmake/help/latest/command/option.html) and [cache entries](https://cmake.org/cmake/help/latest/command/set.html#set-cache-entry), which will not override any (regular or cached) values you set in your `local.cmake` file.[^2]

[^2]: However, once you remove your custom setting, beware that cache entries retain their values from previous invocations and may be different from what's listed in [build/defaults.cmake](build/defaults.cmake).

### Overriding settings on invocation

CMake cache entries can be overridden on the command line, e.g. via `make` as follows

  ```shell
  make PRODUCT="my-new-product" BUILD_NUMBER=42 BUILD_PYTHON_WHEELS=ON
  ```

These are then converted to corresponding cmake arguments `-D VARIABLE=VALUE` in  [Makefile](Makefile).


## Writing code

To learn how to contribute, either by making updates to an existing application or by creating a new one, you may want to start with the [demo](src/mantle/demo) application and its [documentation](src/mantle/demo/README.md).

Also look at the [C++ Style Guide](docs/developing/style-guide-cpp.md) for coding conventions used in this repository.


Questions? Comments? Concerns?
------------------------------

Please get in touch:

> Tor Slettnes  
> tor@slett.net  
> +1.925.8888.TOR
