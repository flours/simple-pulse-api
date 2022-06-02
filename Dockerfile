FROM centos:7

WORKDIR /root
RUN yum update -y && yum install -y git
RUN curl -sL https://rpm.nodesource.com/setup_14.x | bash - && yum install -y nodejs

# python3.9
RUN yum install cmake gcc -y

RUN yum groupinstall "Development Tools" -y
RUN yum install openssl-devel libffi-devel bzip2-devel wget -y
RUN wget https://www.python.org/ftp/python/3.9.2/Python-3.9.2.tgz
RUN tar xvf Python-3.9.2.tgz
RUN cd Python-3.9*/ && ./configure --enable-optimizations && make altinstall

# install pulseaudio
RUN yum update -y
RUN yum install -y libtool-ltdl-devel libpcap-devel gettext-devel libsndfile-devel m4 wget gcc cc make
RUN yum install -y libcap-devel libcap
RUN yum install -y perl-XML-Parser-2.41-10.el7.x86_64
RUN wget https://www.freedesktop.org/software/pulseaudio/releases/pulseaudio-13.0.tar.xz
RUN xz -dc pulseaudio-13.0.tar.xz | tar xfv -  && rm pulseaudio-13.0.tar.xz
WORKDIR /root/pulseaudio-13.0
RUN ./bootstrap.sh
RUN make && make install && ldconfig 

RUN yum -y update && yum install -y gcc gcc-c++ curl-devel  curl \
 && yum install -y libX11 \
                   GConf2 \
                   fontconfig \
#  && yum install -y google-chrome-stable \
#                   libOSMesa \
  && yum install -y pulseaudio alsa-utils pulseaudio-libs-devel pulseaudio-utils \
  && rm -rf /var/cache/yum/* && yum clean all

RUN sed -i '/load-module module-console-kit/d' /etc/pulse/default.pa \
  && echo 'load-module module-null-sink sink_name=MySinkA' >>  /etc/pulse/default.pa \
  && echo 'load-module module-null-sink sink_name=MySinkB' >>  /etc/pulse/default.pa \
  && echo 'load-module module-virtual-source source_name=MySinkBMic master=MySinkB.monitor' >> /etc/pulse/default.pa \
  && echo 'set-default-sink MySinkA' >>  /etc/pulse/default.pa \
  && echo 'set-default-source MySinkBMic' >>  /etc/pulse/default.pa \
  && echo 'set-sink-volume MySinkA 60000' >>  /etc/pulse/default.pa

RUN npm install -g node-gyp
COPY ./src /root/src
COPY ./binding.gyp /root
COPY ./package.json /root
COPY ./lib /root/lib
WORKDIR /root/
RUN npm install
RUN  node-gyp rebuild --python="/usr/local/bin/python3.9"
