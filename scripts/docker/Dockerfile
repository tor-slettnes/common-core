FROM debian:bookworm

# Set Docker variables for creating picarro user
ARG USERNAME=developer
ARG HOMEDIR=/home/${USERNAME}
ARG USER_UID=1000
ARG USER_GID=$USER_UID

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt -y install \
    build-essential cmake pkgconf uuid-dev \
    locales-all libgtest-dev \
    rapidjson-dev \
    libprotobuf-dev protobuf-compiler \
    protobuf-compiler-grpc libgrpc++-dev \
    cppzmq-dev \
    libcurl4-gnutls-dev \
    libwebsocketpp-dev \
    python3-dev \
    python3-protobuf python3-grpcio python3-zmq python3-cffi-backend \
    doxygen
ENV DEBIAN_FRONTEND=dialog

# Create the user
RUN addgroup --gid ${USER_GID} ${USERNAME}
RUN adduser --uid ${USER_UID} --ingroup ${USERNAME} ${USERNAME}
RUN adduser ${USERNAME} sudo

# Switch to picarro user to set up user environment
USER $USERNAME
WORKDIR /home/${USERNAME}
