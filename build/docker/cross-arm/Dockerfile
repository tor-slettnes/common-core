ARG DOCKERBASE=debian:bookworm
FROM $DOCKERBASE

# Include ARM64 package cataloge in APT updates
RUN dpkg --add-architecture arm64

# Install required build & runtime libraries.
RUN apt-get clean
RUN apt-get update
RUN apt-get install --assume-yes --no-install-recommends \
    crossbuild-essential-arm64 cmake pkgconf file \
    protobuf-compiler protobuf-compiler-grpc \
    rapidjson-dev:arm64 libyaml-dev:arm64 uuid-dev:arm64 \
    libudev-dev:arm64 libnm-dev:arm64 libglibmm-2.4-dev:arm64 \
    libavahi-client-dev:arm64 libsqlite3-dev:arm64 libzmq3-dev:arm64 \
    libcurl4-openssl-dev:arm64 libwebsocketpp-dev:arm64 libasio-dev:arm64 \
    libavro-dev:arm64 librdkafka-dev:arm64 \
    libprotobuf-dev:arm64 libgrpc++-dev:arm64 \
    sudo

ARG USER=builder
ARG UID=1000
ARG GID=1000

RUN groupadd --force --gid "${GID}" "${USER}"
RUN useradd --create-home --non-unique --uid "${UID}" --gid "${USER}" --groups sudo,staff "${USER}"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN echo "%sudo ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo-no-pw

USER ${USER}
CMD ["sleep", "infinity"]
