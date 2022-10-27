# Raspberry Pi setup

## OS Installation

1. Use [Raspberry Pi Imager](https://www.raspberrypi.com/software/)
    to flash each SD card.
    From the imager Select the OS Ubuntu Server 22.04.1 LTS (make
    sure to select the 64-bit server OS for arm64 architecture) or
    Raspberry Pi OS Lite (64-bit).

1. Open the option tag and:
    - Set the hostname
    - Enable SSH
    - use username and password authentication
    - insert username and password

## OS Configuration

1. Connect to the pi with an ethernet cable and log in with default
    credentials if you didn't set a username and password in the
    previous step. default credential:

    ```bash
    ssh pi@raspberrypi.local
    # user: pi
    # pass: raspberry
    ```

    or you can connect using your pi ip address:

    ```bash
    ssh pi@your_pi_ip
    # user: pi
    # pass: raspberry
    ```

1. configure a static ip address (optional)
    - On your PI (Raspian Os Using Ethernet cable):

        ```bash
        sudo nano /etc/dhcpcd.conf
        ```

        add the following lines to the file (the addresses are just an example):

        ```bash
        inerface eth0
        static ip_address = 192.168.3.2/24
        static routers = 192.168.4.1
        static domain_name_servers=192.168.4.1
        ```

    - On your pc (Linux)
        - open your network manager:

        ```bash
        nm-connection-editor
        ```

        - add a new eth0 connection
        - give it a name (eg. rpi)
        - under IPv4 Settings
            - set method to: Shared other computers (this allows
              your rpi to use your pc internet connection)
            - add the ip address: 192.168.4.1
            - the Netmask: 24
            - the Gateway: 192.186.4.1
        - ensure that the system doesn't attempt to use the
          ethernet connection for internet instead of wifi:

        ```bash
        nmcli connection modify rpi ipv4.never-default true
        ```

        (rpi is the connection name we set in the network manager)

    > **TODO**: Ubuntu Server Lts Using Ethernet cable

1. Enable `cgroup`

    - Raspberry PI OS: in the file `/boot/cmdline.txt` add

      ```bash
      cgroup_enable=cpuset cgroup_memory=1 cgroup_enable=memory
      ```

    - Ubuntu Server LTS: in the file `/boot/firmware/cmdline.txt`

      ```bash
      cgroup_enable=cpuset cgroup_memory=1 cgroup_enable=memory
       ```

1. Update the system:

    ```bash
    sudo apt-get update
    sudo apt-get upgrade
    ```

1. Enable I2C and other settings.

    - Raspberry Pi OS

        ```bash
        sudo raspi-config
        ```

    - Go to `Interfacing Options` and enable I2C and SPI.
    - (Optional if you forgot to set them before flashing the sd card):
        - Set the hostname to the device name (e.g. `martel-rbp-0002`)
        - Set the password to `marteldiorama2022`
        - Set locale to `EN_US.UTF-8`
    - Finish and Reboot

## Install Groove.py

1. Make sure I2C is enabled (see Os configuration 4.)

1. Download groove.py and install dependencies:

    ```bash
    git clone https://github.com/Seeed-Studio/grove.py
    cd grove.py
    ```

    - Python2 (This command line does not provide for RaspberryPI
      OS 10 (Buster) >= 2020-12-02)

      ```bash
      sudo pip install .
      ```

    - Python3 :

      ```bash
      sudo pip3 install .
      ```

1. the Groove.py documentation is available [HERE](https://seeed-studio.github.io/grove.py/)

## Install Docker

1. Install curl if you don't have it (curl --version to check)

    ```bash
    sudo apt install -y curl
    ```

1. Push the following installation script.

    ```bash
    curl -fsSL https://get.docker.com -o get-docker.sh
    ```

1. Execute the script

    ```bash
    sudo sh get-docker.sh
    ```

1. Append a non-root user on the Docker group

    ```bash
    ‍sudo usermod -aG docker ${USER}
    ```

## Install Microk8s and Kubedge

### Master node

Advised operating systems: Ubuntu 22.04 LTS, 20.04 LTS, 18.04 LTS or
16.04 LTS environment to run the commands (or another operating
system which supports snapd)

1. Install microk8s

    ```bash
    sudo snap install microk8s --classic
    sudo usermod -a -G microk8s $USER
    sudo chown -f -R $USER ~/.kube
    su - $USER
    microk8s status --wait-ready
    ```

    at this point you should be able to get your nodes with:

    ```bash
    microk8s kubectl get nodes
    ```

    (optional) you can assign an alias to avoid conflicts if you
     don't have previous install of kubectl:

    ```bash
    alias kubectl='microk8s kubectl'
    ```

1. Install Kubeedge master node using Keadm

    ```bash
    docker run --rm kubeedge/installation-package:v1.10.0 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. initialize masternode

    ```bash
    keadm init --advertise-address="your-pc-ip"
    ```

1. Get your master node token (this will be used by the worker
  node to connect to the master node)

    ```bash
    keadm gettoken
    ```

### Worker Node (RaspberryPi)

Advised operating system: Ubuntu Server LTS 64-bit arm-64 but
it will work also for Raspberry Pi OS Lite 64-bit arm-64.

1. Install kubeedge

    ```bash
    docker run --rm kubeedge/installation-package:v1.10.0 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. access as root:

    ```bash
    sudo su -
    ```

1. Create a node and joint the master node

    ```bash
    keadm join --cloudcore-ipport=your-pc-ip:10000 --token=my-cloud-side-token
    ```

    - `your-pc-ip` is the ip address of the pc you are using as a master)
    - `my-cloud-side-token` is the token of the master node see step
      4 in section [Master Node](#Master Node))

1. Output and Fixes

    - the expected output of the previous point should be:

        ```bash
        KubeEdge edgecore is running, For logs visit: journalctl -u edgecore.service -xe
        ```

    - On the Master run the following command:

        ```bash
        microk8s kubectl get nodes
        ```

        - if your slave node appears in the node list congrats you are done!
        - if not continue to the next point

    - On the slave check the journal:

        ```bash
        journalctl -u edgecore.service -xe
        ```

    - If you get the following error:

    ```bash
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.701932 34795 server.go:76] Version: v1.11.1
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.727867 34795 server.go:103] Use node IP address from config: 192.168.1.115
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.736222 34795 sql.go:21] Begin to register twin db model
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.753479 34795 module.go:52] Module twin registered successfully
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.980657 34795 client.go:78] "Connecting to docker on the dockerEndpoint" endpoint="unix:///var/run/docker.sock"
    Sep 29 11:39:47 pim edgecore[34795]: I0929 11:39:47.989949 34795 client.go:97] "Start docker client with request timeout" timeout="0s"
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.607399 34795 docker_service.go:242] "Hairpin mode is set" hairpinMode=hairpin-veth
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.616613 34795 cni.go:239] "Unable to update cni config" err="no networks found in /etc/cni/net.d"
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.693456 34795 hostport_manager.go:72] "The binary conntrack is not installed, this can cause failures in network connection cleanup."
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.693708 34795 hostport_manager.go:72] "The binary conntrack is not installed, this can cause failures in network connection cleanup."
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.771215 34795 docker_service.go:257] "Docker cri networking managed by the network plugin" networkPluginName="kubernetes.io/no-op"
    Sep 29 11:39:48 pim edgecore[34795]: I0929 11:39:48.892760 34795 docker_service.go:263] "Docker Info" dockerInfo=&{ID:SGBF:Q2AK:TQEA:DZ6G:M5S7:LEVV:553O:KKRJ:W45Z:3TY4:GI6Q:4LQ3 Containers:0 ContainersRunning:0 Cont>
    Sep 29 11:39:48 pim edgecore[34795]: E0929 11:39:48.893016 34795 edged.go:279] init new edged error, misconfiguration: kubelet cgroup driver: "cgroupfs" is different from docker cgroup driver: "systemd"
    Sep 29 11:39:48 pim systemd[1]: edgecore.service: Main process exited, code=exited, status=1/FAILURE
    ```

    it means the `cgroup` of docker and kubeedge are in conflict,
    to fix this error you need to edit the `edgecore.yaml` file:

    ```bash
    nano /etc/kubeedge/config/edgecore.yaml
    ```

    replace: `cgroupDriver: cgroupfs` with: `cgroupDriver: systemd`,
    the worker node should have joined the cluster, on the master
    verify with

    ```bash
    microk8s kubectl get nodes
    ```

1. if it didn't work you probably used a wrong ip or port, run:

    ```bash
    rm /etc/systemd/system/edgecore.service
    ```

    and go back to step 3.
