FROM debian:unstable

RUN apt update -y && apt install build-essential net-tools emacs ethtool arping -y

RUN mkdir -p /dev/net && \
    mknod /dev/net/tun c 10 200 && \
    chmod 600 /dev/net/tun