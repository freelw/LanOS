FROM ubuntu:18.04

RUN  apt update \
	&& apt-get -y install nasm gcc make

COPY entrypoint.sh /root/entrypoint.sh
WORKDIR /root/
ENTRYPOINT ["/root/entrypoint.sh"]
