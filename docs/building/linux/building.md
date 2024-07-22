Building
--------

If you haven't already done so, please first go through the previous section: [Preparing Your Build Environment](preparing.md) and, if applicable, [Obtaining the Source](cloning.md).


## Build and Install in place

### Build and Install

Now that you have prepared your build environment, navigate to the root of your workspace (where you cloned this repository) and invoke:

  ```shell
  make develop
  ```

Or simply the following, as `develop` is the default [Makefile](../../../Makefile) target:

  ```shell
  make
  ```

This will
  * run `cmake -B ${BUILD_DIR}`, where `${BUILD_DIR}` is `out/build/${TARGET}-${BUILD_TYPE}` (e.g. `out/build/Linux-x86_64-Release`). This folder is created if needed.
  * run `cmake --build ${BUILD_DIR}` to perform the actual build
  * run `ctest` within the build directory to run available unit tests
  * run `cmake --install ${BUILD_DIR} --prefix ${INSTALL_DIR} --strip`, where `${INSTALL_DIR}` is `out/install/${TARGET}`.  This will install the build outputs within your own working directory


If all went well the applications will now be installed within the the installation folder, `${INSTALL_DIR}`:
  * Server applications go under `sbin`
  * User-accessible applications go under `bin`
  * Python scripts go under `lib/python3/dist-packages`
  * Settings files go under `share/cc/settings`.


### Run in place

Applications can now be launched from within `${INSTALL_DIR}`.  For instance if you built the DEMO example (check the option `BUILD_DEMO` in [CMakeLists.txt](../../../CMakeLists.txt)), you can now launch it using:

  ```shell
  ./out/install/Linux-x86_64/sbin/demoserver
  ```

  (**Hint:** Use the `--help` option for usage).


In a separate terminal can also launch the Python interactive test utility, `demoshell`:

  ```shell
  ./out/install/Linux-x86_64/bin/demoshell
  ```


## Create Installation Packages

Rather than "installing" the build outputs in your local `out/install`, you can create release packages that can then be installed onto any compatible Debian or Ubuntu target system.  To do so, invoke

   ```shell
   make release
   ```

This will also run unit tests, as the `release` target depends on `test` and `package`. To build *without* running the unit tests, invoke

   ```shell
   make package
   ```

The outputs will be:

- one or more interdependent Debian packages (`.deb` files) that you can install either onto your own host environment or onto a separate Debian-based target system

- one or more [Python wheels](https://pythonwheels.com/) (`.whl` files) that you can use in a Python 3.9 or higher environment on any platform


#### About components

Whther the above creates a single `.deb` package containing all installable components, or a separate `.deb` file for each. This behavior is controlled by the option `CPACK_DEB_COMPONENT_INSTALL` found in any of these places (in order of priority):

1. `CPackConfig.cmake` within the top-level folder of your project root (if you are using `common-core` as a submodule within another repository)
2. [CPackConfig.cmake](../../../CPackConfig.cmake) within [common-core](../../..)
3. [cmake/CPackConfig.cmake](../../../cmake/CPackConfig) also within [common-core](../../..)

Throughout the code in this repository, install components are is identified using the `COMPONENT` option to the CMake [install()](https://cmake.org/cmake/help/latest/module/CPackComponent.html#command:cpack_add_component) command, or indirectly with our own wrapper methods such as [BuildExecutable()](../../../cmake/BuildExecutable.cmake), [BuildPython()](../../../cmake/BuildPython.cmake), etc.  Detailed definitions for each component are provided using the CMake command [cpack_add_component()](https://cmake.org/cmake/help/latest/module/CPackComponent.html#command:cpack_add_component)

#### Package name

The Debian package name (or the prefix if you're creating multiple packages as described above) is specified with the option `CPACK_PACKAGE_NAME` (again in `CPackConfig.cmake`; see above for resolution order).



## A Note on Versioning

The release version used both within the applications and the release package itself is set in the top-level `CMakeLists.txt` file, using the following priority order:

1. If the non-empty environment variable `VERSION` exists, use it and cached it for future invocations. It can be set by supplying it to the `make` commands above, e.g.,

   ```shell
   make package VERSION=1.2.3
   ```

2. Otherwise, if `VERSION` is alredy cached from a previous invocation, or from a parent project, use that.

3. Otherwise, read the default version from the file `.version` within the top-level project directory.


**Note**: the version number is _not_ written back into the `.version` file, on the assumption that any changes commited to GIT should be made by explicitly editing the source. This means that the supplied version number will be reset after any subsequent invocation of `make pristine`.



## Next steps

Once you have successfully built the `.deb` and `.whl` installation packages, proceed to [Installing and Running](running.md).
