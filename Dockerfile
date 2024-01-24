FROM ubuntu:22.04

RUN \
	apt update && \
	apt install -y wget clang make libsodium-dev libopus0 && \
	wget -O dpp.deb https://dl.dpp.dev/ && \
	dpkg -i dpp.deb && \
	rm dpp.deb && \
	apt clean

WORKDIR /app

COPY src src
COPY Makefile Makefile

RUN make

CMD make run
