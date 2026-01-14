ARG DOCKERBASE=debian:bookworm
FROM $DOCKERBASE

# Install required build & runtime libraries.
#
# Split `apt-get` commands into logical groups, so that Docker cache misses
# after future changes don't trigger a full download.

RUN apt-get clean
RUN apt-get update

RUN apt-get install -y sudo file
RUN apt-get install -y git build-essential cmake pkgconf doxygen
#RUN apt-get install -y crossbuild-essential-arm64 dpkg-cross

# python3-cffi-backend
# python3-avro
# python3-kafka
# python3-venv

ARG USER
ARG UID
ARG GID

# Remove preinstalled `ubuntu` user (if we are using an Ubuntu base),
# and replace with our own
RUN userdel --force --remove ubuntu || true
RUN groupdel --force --remove ubuntu || true

RUN groupadd --force --gid "${GID}" "${USER}"
RUN useradd --create-home --non-unique --uid "${UID}" --gid "${USER}" --groups sudo,staff "${USER}"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN echo "%sudo ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo-no-pw

USER ${USER}
CMD ["sleep", "infinity"]
