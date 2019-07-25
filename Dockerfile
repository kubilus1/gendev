FROM ubuntu:16.04

WORKDIR /tmp

RUN apt-get update && apt-get install -y build-essential wget unzip unrar-free texinfo git default-jdk-headless && apt-get clean

RUN bash -c "git clone https://github.com/kubilus1/gendev.git && cd gendev && make && make sgdk_samples && rm -rf /tmp/*"

CMD /bin/bash
