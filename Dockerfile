FROM 32bit/ubuntu:14.04

WORKDIR /tmp

RUN apt-get update && apt-get install -y build-essential wget unzip unrar texinfo git subversion && apt-get clean

RUN bash -c "git clone https://github.com/kubilus1/gendev.git && cd gendev && make && rm -rf /tmp/*"

CMD /bin/bash
