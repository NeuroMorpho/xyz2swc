# our base image
FROM ubuntu:bionic-20230301

# Install python and pip
LABEL maintainer="bljungqu@gmu.edu"

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN dpkg --add-architecture i386
RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y dist-upgrade

RUN apt-get -y install software-properties-common

RUN apt-get -y install --reinstall ca-certificates
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update

RUN apt-get -y install libhdf5-cpp-100:i386 libhdf5-dev:i386 libhdf5-100:i386 zlib1g:i386 zlib1g-dev:i386 libsz2:i386 libc6-dev-i386 libdlib18:i386 libdlib-dev:i386 make:i386 python3-pip
RUN apt-get -y install gcc-11 g++-11 gcc-11-multilib g++-11-multilib python3.8 python3.8-dev python3.8-distutils python3.8-venv 

#RUN apt-get -y install libssl1.1


WORKDIR /app

COPY modules/neuronland/3rdParty/szip/szip-2.1.1 /app/lib/szip-2.1.1
WORKDIR /app/lib/szip-2.1.1
RUN ./configure -build=i686-pc-linux-gnu --host=i686-pc-linux-gnu CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32
RUN make && make check && make install
COPY modules/neuronland /app/lib/neuronland
WORKDIR /app/lib/neuronland
RUN ./GenerateBuildFiles_gmake2.sh
WORKDIR /app/lib/neuronland/solution_gmake2
RUN ln -s /usr/lib/i386-linux-gnu/hdf5/serial/libhdf5.so /usr/lib/libhdf5.so
RUN rm /usr/bin/gcc
RUN ln -s /usr/bin/gcc-11 /usr/bin/gcc
RUN ln -s /usr/bin/g++-11 /usr/bin/g++
RUN make
RUN ln -s /app/lib/neuronland/solution_gmake2/bin/x86/Debug/NeuronMorphologyFormatConverter /usr/bin/nmoc


WORKDIR /app


## R

RUN gpg --keyserver keyserver.ubuntu.com --recv-key E298A3A825C0D65DFD57CBB651716619E084DAB9
RUN gpg -a --export E298A3A825C0D65DFD57CBB651716619E084DAB9 | apt-key add -
RUN add-apt-repository 'deb https://cloud.r-project.org/bin/linux/ubuntu bionic-cran40/' && apt-get update && apt-get -y install r-base
RUN Rscript -e 'install.packages("nat", dependencies=TRUE)'

## Matlab runtime
COPY lib/matlab_rt.zip /app/lib/
WORKDIR /app/lib
RUN unzip matlab_rt.zip
WORKDIR /app
COPY modules/ndf /app/modules/ndf
RUN lib/matlab_rt/install -mode silent -agreeToLicense yes


COPY requirements.txt /app/
#RUN python -m pip install -U setuptools

RUN rm /usr/bin/python3
RUN ln -s /usr/bin/python3.8 /usr/bin/python3

RUN python3 -m pip install -U pip
RUN pip3 install pybind11
RUN pip3 install --no-cache-dir -r /app/requirements.txt


## Fiji snt 
RUN apt-get update
RUN apt-get -y install openjdk-11-jdk
COPY lib/ImageJ-linux64 /app/lib/
COPY modules/snt/ /app/modules/snt

RUN apt-get -y install nodejs npm

WORKDIR /app/modules/snt
RUN unzip fiji-linux64.zip
RUN mkdir /app/modules/snt/logfiles/

#RUN apt-get -y install maven

COPY modules/mtr/ /app/modules/mtr
#Amira
COPY modules/am/ /app/modules/am
#RUN mkdir /app/modules/am/logfiles/

COPY modules/hbp/ /app/modules/hbp

WORKDIR /app
COPY app.py /app/
COPY /xyz2swc /app/xyz2swc/
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

COPY input /app/input

COPY ui /app/ui


# tell the port number the container should expose
EXPOSE 8001

# run the application
#CMD ['ping', '-c', '3', 'google.com']
CMD ["python3.8", "-m", "uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8001", "--reload", "--log-level", "debug"]