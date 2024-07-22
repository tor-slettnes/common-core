#!/bin/bash -e
################################################################################
## @file create_schroot.sh
## @brief Create `schroot` environment in which to build & run
## @author Tor Slettnes <tor@slett.net>
################################################################################

NAME="common-core"
DESC="Common Core Build System"
SUITE="bookworm"
COMPONENTS="main"
MIRROR="https://deb.debian.org/debian"
ROOT="/srv/chroot/${NAME}"
TESTFILE="${ROOT}/usr/bin/dpkg"

REQS=(
    # Basic build utilities
    build-essential cmake pkgconf git

    # `uuid()` generator
    uuid-dev

    # Fast JSON parser
    rapidjson-dev

    # GoogleTest
    libgtest-dev locales-all

    # Google Protocol Buffers
    libprotobuf-dev protobuf-compiler

    # gRPC Remote Procedure Calls
    protobuf-compiler-grpc libgrpc++-dev

    # Send/receive messages over ZeroMQ
    cppzmq-dev

    # LibCURL
    libcurl4-gnutls-dev

    # Websockets
    libwebsocketpp-dev

    # D-Bus integration
    libglibmm-2.4-dev

    # Network Manager interface
    libnm-dev

    # UDev interface
    libudev-dev

    # Embedded Python environment (e.g. for pickling/unpickling)
    python3-dev

    # Python modules to create installable wheels
    python3-build python3-virtualenv python3-pip python3-hatchling

    # Python source modules for ProtoBuf, gRPC, and ZeroMQ
    python3-protobuf python3-grpcio python3-zmq python3-cffi-backend

    # Generate reference documents
    doxygen
)


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

host_reqs=(schroot debootstrap)
if ! dpkg --status "${host_reqs[@]}" >/dev/null 2>&1
then
    echo ""
    echo "### Installing required packages: ${host_reqs[@]}"
    $run_as_root apt -q update || true
    $run_as_root apt -q -y install "${host_reqs[@]}"
fi

if [[ ! -e "/etc/schroot/chroot.d/${NAME}" ]]
then
    echo ""
    echo "### Creating 'schroot' instance: ${NAME}"
    $run_as_root dd of="/etc/schroot/chroot.d/${NAME}" status=none <<EOF
[${NAME}]
type=directory
directory=/srv/chroot/${NAME}
description=${DESC}
groups=staff,root
aliases=default,${SUITE}
EOF
fi

if [[ ! -e "${TESTFILE}" ]]
then
    ### Target does not yet exist it. Create it.
    printf -v reqs "%s," "${REQS[@]}"

    echo ""
    echo "### Retrieving and installing target system via 'debootstrap'"
    $run_as_root /usr/sbin/debootstrap \
                 --components="${COMPONENTS}" \
                 --include="${reqs%,}" \
                 "${SUITE}" "${ROOT}" "${MIRROR}"

elif ! schroot --chroot "${NAME}" -- dpkg --status "${REQS[@]}" >/dev/null 2>&1
then
     ### Target exists but is missing some required packages.  Install them.
     echo ""
     echo "### Target system exists, but is missing packages. Installing:"

     $run_as_root schroot --chroot "${NAME}" -- apt -q update || true
     $run_as_root schroot --chroot "${NAME}" -- apt -q -y install "${REQS[@]}"

fi

if [[ $(id -u) == 0 ]]
then
    echo ""
    echo "### Be sure to add any authorized users to the 'staff' group, e.g.:"
    echo ""
    echo "        adduser USERNAME staff"
    echo ""
    echo "    Then, log out and in again for this change to take effect"

elif ! groups | grep -qw staff
then
    echo ""
    echo "### Adding user '$(id -un)' to group 'staff'"
    $run_as_root adduser "$(id -un)" staff
    echo ""
    echo "    Please log out and in again for this change to take effect"

fi

echo ""
echo "### All done!"
echo ""

