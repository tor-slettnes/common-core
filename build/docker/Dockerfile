ARG DOCKERBASE=debian:bookworm
FROM $DOCKERBASE

RUN apt-get clean
RUN apt-get update
RUN apt-get install --assume-yes sudo make

ARG USER
ARG UID
ARG GID
ARG DEBS

# Remove preinstalled `ubuntu` user (if we are using an Ubuntu base),
# and replace with our own
RUN userdel --force --remove ubuntu || true
RUN groupdel --force --remove ubuntu || true

RUN groupadd --force --gid "${GID}" "${USER}"
RUN useradd --create-home --non-unique --uid "${UID}" --gid "${USER}" --groups sudo,staff "${USER}"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN echo "%sudo ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo-no-pw

RUN apt install --yes --no-install-recommends ${DEBS}

USER ${USER}
CMD ["sleep", "infinity"]
