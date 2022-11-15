FROM navikey/raspbian-buster
ENV DEBIAN_FRONTEND noninteractive
ARG USER=pi
ARG UID=1000
ARG GID=100
RUN apt-get -y update && apt-get -y install curl git sudo
# Set environment variables
ENV USER                ${USER}
ENV HOME                /home/${USER}
RUN useradd -m -s /bin/bash -N -u $UID $USER && \
    echo "${USER} ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers && \
    chmod 0440 /etc/sudoers && \
    chmod g+w /etc/passwd 
RUN groupadd $USER
RUN usermod -aG $USER $USER
RUN apt-get -y install python3 lsb-release
RUN apt-get -y install python3-pip

WORKDIR $HOME
USER ${UID}

RUN curl -kL dexterindustries.com/update_grovepi | bash -s -- --bypass-gui-installation