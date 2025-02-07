FROM debian:bookworm
#FROM ubuntu:22.04

### Install runtime utilities
RUN apt-get clean
RUN apt-get update
RUN apt-get install -y \
    sudo                    \
    build-essential         \
    cmake                   \
    doxygen                 \
    file                    \
    git                     \
    uuid-dev                \
    rapidjson-dev           \
    libyaml-dev             \
    libsqlite3-dev          \
    libavahi-client-dev     \
    libzmq3-dev             \
    libcurl4-gnutls-dev     \
    libwebsocketpp-dev      \
    libprotobuf-dev         \
    libgrpc++-dev           \
    libavro-dev             \
    librdkafka-dev          \
    libudev-dev             \
    libglibmm-2.4-dev       \
    libnm-dev               \
    libgtest-dev            \
    locales-all             \
    pkgconf                 \
    protobuf-compiler       \
    protobuf-compiler-grpc  \
    python3-virtualenv      \
    python3-venv            \
    python3-build           \
    python3-hatchling       \
    python3-dev             \
    python3-pytest          \
    python3-cffi-backend    \
    python3-yaml            \
    python3-grpcio          \
    python3-protobuf        \
    python3-zmq             \
    python3-avro            \
    python3-kafka           \

# Duplicate external account

ARG USERNAME
ARG UID
ARG GID

RUN adduser --disabled-password $USERNAME --uid $UID
RUN adduser $USERNAME sudo

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN sed -e 's/\(%sudo\s\+.\+\)\s\+\(ALL\)$/\1 NOPASSWD:\2/g' -i /etc/sudoers

USER $USERNAME

CMD ["sleep", "infinity"]
