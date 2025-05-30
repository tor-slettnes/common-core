FROM debian:bookworm
#FROM ubuntu:22.04

### Install runtime utilities
RUN apt-get clean
RUN apt-get update
RUN apt-get install -y \
    build-essential         \
    cmake                   \
    doxygen                 \
    file                    \
    git                     \
    libavahi-client-dev     \
    libavro-dev             \
    libcurl4-gnutls-dev     \
    libglibmm-2.4-dev       \
    libgrpc++-dev           \
    libgtest-dev            \
    libnm-dev               \
    libprotobuf-dev         \
    librdkafka-dev          \
    libsqlite3-dev          \
    libudev-dev             \
    libwebsocketpp-dev      \
    libyaml-dev             \
    libzmq3-dev             \
    locales-all             \
    pkgconf                 \
    protobuf-compiler       \
    protobuf-compiler-grpc  \
    python3-avro            \
    python3-build           \
    python3-cffi-backend    \
    python3-dev             \
    python3-grpcio          \
    python3-hatchling       \
    python3-kafka           \
    python3-pip             \
    python3-protobuf        \
    python3-pytest          \
    python3-venv            \
    python3-virtualenv      \
    python3-yaml            \
    python3-zmq             \
    rapidjson-dev           \
    sudo                    \
    twine                   \
    uuid-dev

# Duplicate external account

ARG USERNAME
ARG UID
ARG GID

RUN useradd --create-home --uid "$UID" --groups sudo,staff "$USERNAME"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN sed -e 's/\(%sudo\s\+.\+\)\s\+\(ALL\)$/\1 NOPASSWD:\2/g' -i /etc/sudoers

RUN mkdir -p -m 2775 /var/log/common-core
RUN chgrp staff /var/log/common-core

USER $USERNAME

CMD ["sleep", "infinity"]
