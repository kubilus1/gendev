FROM ubuntu:18.04

WORKDIR /tmp

RUN apt-get update && \
    apt-get install -y build-essential wget unzip unrar-free texinfo git openjdk-8-jdk dos2unix && \
    apt-get clean

# To download gcc 6.3 with wget. We must provide a password with '@' in.
RUN echo ftp_user=anonymous > ~/.wgetrc
RUN echo ftp_password=dfdf@dfdf.com >> ~/.wgetrc

RUN bash -c "git clone https://github.com/kubilus1/gendev.git && cd gendev && make && rm -rf /tmp/*"

CMD /bin/bash
