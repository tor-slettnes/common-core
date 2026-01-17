ARG DOCKERBASE=debian:bookworm
FROM $DOCKERBASE

RUN apt-get clean
RUN apt-get update
RUN apt-get install --assume-yes sudo make

ARG USER
ARG UID
ARG GID
ARG DEBS

RUN groupadd --force --gid "${GID}" "${USER}"
RUN useradd --create-home --non-unique --uid "${UID}" --gid "${USER}" --groups sudo,staff "${USER}"

# Do not require password for users in the `sudo` group to invoke `sudo`
RUN echo "%sudo ALL=(ALL:ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo-no-pw

RUN apt-get install --yes --no-install-recommends ${DEBS}

USER ${USER}
CMD ["sleep", "infinity"]
