FROM orchestracities/grovepi:3.7-buster
#### Python
# Install pip requirements
USER root
ADD requirements.txt .
RUN sudo apt install python2
RUN python2 -m pip install -r requirements.txt
RUN python3 -m pip install -r requirements.txt
USER pi
WORKDIR /app
ADD . /app

# User needs to be root to be able to access GPIO.

# During debugging, this entry point will be overridden. For more information, please refer to https://aka.ms/vscode-docker-python-debug
CMD ["sudo", "python", "parking.py", "&", "python", "ev-charging/ev-charging.py"]