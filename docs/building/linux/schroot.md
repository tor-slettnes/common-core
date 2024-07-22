Building in a `chroot` environment
==================================

If you have an older Linux system and/or you want to use a separate build environment for this framework, you can do so using the provided [create_schroot.sh](../../../scripts/create_schroot.sh) script:

  ```shell
  ./script/create_schroot.sh
  ```

This basically creates a `chroot` environment with automatic mounting/unmounting of various system folders including `/home` as you enter and exit.

Once this script completes, you can enter the `chroot` shell using

  ```shell
  schroot -c common-core
  ```

Or, because the script also creates a `default` alias, you can simply type

  ```shell
  schroot
  ```

From here you can invoke build commands as described in [Building](building.md), e.g.,

  ```shell
  make
  ```

Alternatively you can invoke the build directly from your host environment by prefixing the command with `schroot`:

  ```shell
  schroot make
  ```
