# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set environment variables to prevent interactive prompts during package installation
ARG DEBIAN_FRONTEND=noninteractive

# Update package list and install necessary tools and dependencies
RUN apt-get update && \
    apt-get install -y \
    git \
    curl \
    gnupg \
    make \
    build-essential \
    pkg-config \
    libzmq3-dev \
    protobuf-c-compiler \
    libprotobuf-c-dev \
    libcjson-dev \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Install roverlib 
RUN git clone https://github.com/VU-ASE/roverlib-c.git && \
    cd roverlib-c && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    cd ../.. && \
    rm -rf roverlib-c

# Create a non-root user with a home directory and bash as default shell
ARG USERNAME=devuser
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m -s /bin/bash $USERNAME \
    && apt-get update && apt-get install -y sudo \
    && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Copy skeleton files to set up the user's shell environment
RUN cp /etc/skel/.bashrc /home/$USERNAME/.bashrc \
    && cp /etc/skel/.profile /home/$USERNAME/.profile \
    && chown -R $USERNAME:$USERNAME /home/$USERNAME

# Switch to the new user
USER $USERNAME

# Set the working directory
WORKDIR /home/$USERNAME

# Include shared objects in the library path
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
