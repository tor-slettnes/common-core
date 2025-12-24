ARG DOCKERBASE=debian:bookworm
FROM $DOCKERBASE

# Install required build & runtime libraries.
#
# Split `apt-get` commands into logical groups, so that Docker cache misses
# after future changes don't trigger a full download.

RUN apt-get clean
RUN apt-get update

RUN apt-get install -y sudo file git
RUN apt-get install -y build-essential cmake pkgconf doxygen
RUN apt-get install -y locales-all libgtest-dev
RUN apt-get install -y rapidjson-dev libyaml-dev uuid-dev
RUN apt-get install -y libudev-dev libnm-dev libglibmm-2.4-dev
RUN apt-get install -y libavahi-client-dev libsqlite3-dev
RUN apt-get install -y libzmq3-dev libwebsocketpp-dev libcurl4-gnutls-dev
RUN apt-get install -y libprotobuf-dev protobuf-compiler
RUN apt-get install -y libgrpc++-dev protobuf-compiler-grpc
RUN apt-get install -y libavro-dev librdkafka-dev
RUN apt-get install -y python3-dev

RUN apt-get install -y python3-yaml python3-zmq
RUN apt-get install -y python3-protobuf python3-grpcio
RUN apt-get install -y python3-pip python3-virtualenv python3-build python3-hatchling twine
RUN apt-get install -y python3-pytest

# python3-cffi-backend
# python3-avro
# python3-kafka
# python3-venv

ARG USERNAME
ARG UID

# Remove preinstalled `ubuntu` user (if we are using an Ubuntu base),
# and replace with our own

RUN userdel --force --remove ubuntu || true
RUN useradd --create-home --uid "${UID}" --groups sudo,staff "${USERNAME}"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN echo "%sudo ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo-no-pw

USER ${USERNAME}
CMD ["sleep", "infinity"]
