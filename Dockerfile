FROM debian:unstable

RUN apt update -y && apt install build-essential emacs -y