FROM ubuntu:22.04

RUN apt update
RUN apt install -y wget clang make libsodium-dev libopus0
RUN wget -O dpp.deb https://dl.dpp.dev/
RUN dpkg -i dpp.deb
RUN rm dpp.deb
RUN apt clean

RUN mkdir /app
WORKDIR /app

COPY src src
COPY Makefile Makefile

RUN make

CMD make run
