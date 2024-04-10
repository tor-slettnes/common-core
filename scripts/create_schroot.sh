#!/bin/bash -e
################################################################################
## @file create_schroot.sh
## @brief Create `schroot` environment in which to build & run
## @author Tor Slettnes <tor@slett.net>
################################################################################

NAME=picarro-build
DESC="Picarro build system"
SUITE=bookworm
COMPONENTS="main"
MIRROR="https://deb.debian.org/debian"
ROOT=/srv/chroot/${NAME}

if [[ ! -e /etc/debian_version ]]
then
   echo "This script can only be used on a Debian-based system (incl. Ubuntu)." >&2
   exit 1
fi

if [[ $(id -u) == 0 ]]
then
    run_as_root=
else
    run_as_root=sudo
fi

required_packages=(schroot debootstrap)
if ! dpkg --status "${required_packages[@]}" >/dev/null 2>&1
then
    echo ""
    echo "### Installing required packags: ${required_packages[@]}"
    $run_as_root apt -q update
    $run_as_root apt -q -y install "${required_packages[@]}"
fi

if [[ ! -e "/etc/schroot/chroot.d/${NAME}" ]]
then
    echo ""
    echo "### Creating 'schroot' instance: ${NAME}"
    $run_as_root dd of="/etc/schroot/chroot.d/${NAME}" status=none <<EOF
[picarro-build]
type=directory
directory=/srv/chroot/${NAME}
description=${DESC}
groups=staff,root
aliases=default,build,debian
EOF
fi

if [[ ! -x ${ROOT}/usr/bin/g++ ]]
then
    build_reqs="build-essential,cmake,pkgconf,uuid-dev"
    build_reqs+=",locales-all,libgtest-dev"
    build_reqs+=",rapidjson-dev"
    build_reqs+=",libprotobuf-dev,protobuf-compiler"
    build_reqs+=",protobuf-compiler-grpc,libgrpc++-dev"
    build_reqs+=",cppzmq-dev"
    build_reqs+=",libcurl4-gnutls-dev"
    build_reqs+=",python3-dev"
    build_reqs+=",python3-protobuf,python3-grpcio,python3-zmq,python3-cffi-backend"
    build_reqs+=",doxygen"


    echo ""
    echo "### Retrieving and installing target system via 'debootstrap'"
    $run_as_root debootstrap \
                 --components="${COMPONENTS}" \
                 --include="${build_reqs}" \
                 "${SUITE}" "${ROOT}" "${MIRROR}"
fi

if [[ $(id -u) == 0 ]]
then
    echo ""
    echo "### Be sure to add any authorized users to the 'staff' group, e.g.:"
    echo ""
    echo "        adduser USERNAME staff"
    echo ""
    echo "    Then, log out and in again for the changes to take effect"

elif ! groups | grep -qw staff
then
    echo ""
    echo "### Adding user '$(id -un)' to group 'staff'"
    $run_as_root adduser "$(id -un)" staff

fi

echo ""
echo "### All done!"
echo ""

