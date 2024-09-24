Getting Started
===============

Contents
--------

This repository contains source for the Common Core framework. Most of the code is developed in C++, but there is also client modules written in Python.

Directory Structure
-------------------

This repository is organized as follows:

* [docs](docs) -- Developer documentation
* [cmake](cmake) -- CMake build recipes & include files
* [scripts](scripts) -- Miscellaneious scripts used for configuration, building & testing.
* [src](src) -- top-level source directory; see [source](src/README.md) for details.
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

For instructions on setting up your host environment, cloning, and building the code in this repository, please choose your desired target platform:

* [Getting Started on Linux](docs/building/linux/README.md)
* [Getting Started on Windows](docs/building/windows/README.md)


## Writing code

To learn how to contribute, either by making updates to an existing application or by creating a new one, you may want to start with the [demo](src/mantle/demo) application and its [documentation](src/mantle/demo/README.md).

Also look at the [C++ Style Guide](docs/developing/style-guide-cpp.md) for coding conventions used in this repository.


Questions? Comments? Concerns?
------------------------------

Please get in touch:

> Tor Slettnes  
> tor@slett.net  
> +1.925.8888.TOR
