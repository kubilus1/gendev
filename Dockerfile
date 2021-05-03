FROM ubuntu:bionic as build

ENV DEBIAN_FRONTEND noninteractive

ENV GENDEV /opt/gendev

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        wget \
        unzip \
        unrar-free \
        texinfo \
        git \
        openjdk-8-jdk-headless && \
    apt-get clean

# To download gcc 6.3 with wget. We must provide a password with '@' in.
#RUN echo ftp_user=anonymous > ~/.wgetrc
#RUN echo ftp_password=dfdf@dfdf.com >> ~/.wgetrc

#RUN bash -c "git clone https://github.com/kubilus1/gendev.git && cd gendev && make && rm -rf /tmp/*"
WORKDIR /work
COPY tools /work/tools/
COPY Makefile /work/
COPY sgdk /work/sgdk/
COPY toolchain /work/toolchain/
RUN make
RUN make install

FROM ubuntu:bionic
RUN apt-get update && \
    apt-get install -y \
        openjdk-8-jre-headless \
        build-essential \
        make && \
    apt-get clean

ENV GENDEV /opt/gendev
COPY --from=build /opt/gendev $GENDEV
ENV PATH $GENDEV/bin:$PATH

WORKDIR /src

ENTRYPOINT make -f $GENDEV/sgdk/mkfiles/Makefile.rom 
