# Raspberry Pi setup

## OS Installation

1. Use [Raspberry Pi Imager](https://www.raspberrypi.com/software/)
    to flash each SD card.
    From the imager Select `Raspberry Pi OS (Other)`
    and then from the list pick `Raspberry Pi OS Lite (Legacy)`.

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
    $ ssh pi@raspberrypi.local
    # user: pi
    # pass: raspberry
    ```

    or you can connect using your pi ip address:

    ```bash
    $ ssh pi@your_pi_ip
    # user: pi
    # pass: raspberry
    ```

1. configure a static ip address (optional)

    ```bash
    $ sudo nano /etc/dhcpcd.conf
    ```

    add the following lines to the file (the addresses are just an example):

    ```bash
    inerface eth0
    static ip_address = 192.168.3.2/24
    static routers = 192.168.4.1
    static domain_name_servers=192.168.4.1
    ```

1. Enable `cgroup`: in the file `/boot/cmdline.txt` add
      `cgroup_enable=cpuset cgroup_memory=1 cgroup_enable=memory`

1. Update the system:

    ```bash
    $ sudo apt-get update
    $ sudo apt-get upgrade
    ```

1. Enable I2C and other settings:

    ```bash
    $ sudo raspi-config
    ```

    - Go to `Interfacing Options` and enable `I2C` and `SPI`.
    - (Optional if you forgot to set them before flashing the sd card):
        - Set the hostname to the device name (e.g. `martel-rbp-0002`)
        - Set the password to `marteldiorama2022`
        - Set locale to `EN_US.UTF-8`
    - Finish and Reboot:

      ```bash
      $ sudo reboot
      ```

## Install GrovePI

1. Test `I2C` is correctly enabled (see [OS Configuration](OS_Configuration)):

    ```bash
    $ i2cdetect -y 1
         0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    00:          -- 04 -- -- -- -- -- -- -- -- -- -- -- 
    10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    70: -- -- -- -- -- -- -- --   
    ```

  If you don't see `04` under the column `4` row `00` something went wrong.

1. Install GrovePI:

    ```bash
    $ curl -kL dexterindustries.com/update_grovepi | bash -s -- --bypass-gui-installation
    ```

### Test GrovePI (optional)

1. Download GrovePI project:

    ```bash
    $ cd Dexter
    $ git clone https://github.com/DexterInd/GrovePi
    ```

1. Connect a LED to Digital Port 4 or your GrovePI Hat.

1. Test the LED blinking script:

    ```bash
    $ cd GrovePi/Software/Python/
    $ python grove_led_blink.py
    ```

## Install Docker

1. Install curl if needed (`curl --version` to check)

    ```bash
    $ sudo apt install -y curl
    ```

1. Push the following installation script.

    ```bash
    $ curl -fsSL https://get.docker.com -o get-docker.sh
    ```

1. Execute the script

    ```bash
    $ sudo sh get-docker.sh
    ```

1. Append a non-root user on the Docker group

    ```bash
    $ sudo usermod -aG docker ${USER}
    ```

### Test Docker (optional)

  ```bash
  $ docker run hello-world
  ```

## Install Microk8s and Kubedge

### Master node

Advised operating systems: Ubuntu 22.04 LTS, 20.04 LTS, 18.04 LTS or
16.04 LTS environment to run the commands (or another operating
system which supports snapd)

1. Install microk8s

    ```bash
    $ sudo snap install microk8s --classic
    $ sudo usermod -a -G microk8s $USER
    $ sudo chown -f -R $USER ~/.kube
    $ su - $USER
    $ microk8s status --wait-ready
    ```

    at this point you should be able to get your nodes with:

    ```bash
    $ microk8s kubectl get nodes
    ```

    generate the configuration for kubectl:

    ```bash
    $ sudo su
    $ mkdir /root/.kube
    $ microk8s config > /root/.kube/config
    ```

    (optional) you can assign an alias to avoid conflicts if you
     don't have previous install of kubectl:

    ```bash
    $ alias kubectl='microk8s kubectl'
    ```

1. Install Kubeedge master node using Keadm

    ```bash
    $ docker run --rm kubeedge/installation-package:v1.11.2 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. Initialize master node

    ```bash
    $ keadm init --advertise-address="your-pc-ip"
    ```

1. Complete the configuration enabling the `dynamicController`:

    ```bash
    $ kubectl -n kubeedge edit cm cloudcore

    modules:
      ...
      dynamicController:
        enable: true
    ...
    ```

1. Get your master node token (this will be used by the worker
  node to connect to the master node)

    ```bash
    $ keadm gettoken
    ```

#### Installing Ubuntu as VM on a Mac M1

To run keadm you need an Ubuntu machine, so it's not enough to have k8s running
on your Mac. One of the simplest options to run Ubuntu on your Mac, it's
installing [multipass](https://multipass.run/).

1. As first step, install the client on your Mac:

    ```bash
    $ brew install multipass
    ```

1. Launch a new virtual machine (with enough resources), e.g.:

    ```bash
    $ multipass launch lts --name microk8s --mem 4G --disk 30G --cpus 4 --network en0
    ```

>>**NOTE**: configure the same network interface you use to reach out the pi,
our you won't be able to reach the VM from the pi.

1. Open a shell in the new vm:

    ```bash
    $  multipass shell microk8s
    ```

1. Set up a password (needed to run `su`):

    ```bash
    $  sudo passwd ubuntu
    ```

1. Install docker in the vm:

    ```bash
    $ sudo mkdir -p /etc/apt/keyrings
    $ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
    $ echo \
    "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
    $ sudo apt-get update
    $ sudo apt-get install docker-ce docker-ce-cli containerd.io docker-compose-plugin
    ```

### Worker Node (RaspberryPi)

Required operating system for GrovePi compatibility:
*Raspberry Pi OS Lite (Legacy)*.

1. Access as root:

    ```bash
    $ sudo su -
    ```

1. Install kubeedge

    ```bash
    $ docker run --rm kubeedge/installation-package:v1.11.2 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. Create a node and joint the master node

    ```bash
    $ keadm join --cloudcore-ipport=your-pc-ip:10000 --token=my-cloud-side-token
    ```

    - `your-pc-ip` is the ip address of the pc you are using as a master)
    - `my-cloud-side-token` is the token of the master node see step
      4 in section [Master Node](#Master Node))

1. To complete the configuration, enable the `metaServer`:

    ```bash
    $ /etc/kubeedge/config/edgecore.yaml
    modules:
      ...
      metaManager:
        metaServer:
          enable: true
    ...
    ```

1. Configure the `clusterDNS` and `clusterDomain`:

    ```bash
    $ /etc/kubeedge/config/edgecore.yaml
    modules:
      ...
      edged:
        clusterDNS: 169.254.96.16
        clusterDomain: cluster.local
    ...
    ```

1. Add filter labels to Kubernetes API services

    ```bash
    $ kubectl label services kubernetes service.edgemesh.kubeedge.io/service-proxy-name=""
    ```

1. Install edgemesh:

    ```bash
    $ export psk=$(openssl rand -base64 24)
    $ microk8s helm3 install edgemesh --namespace kubeedge \
    --set agent.psk=$psk \
    --set agent.relayNodes[0].nodeName=microk8s,agent.relayNodes[0].advertiseAddress="{192.168.1.115}" \
    https://raw.githubusercontent.com/kubeedge/edgemesh/main/build/helm/edgemesh.tgz
    ```

    >>**NOTE**: The `advertiseAddress` is the ip of the master node that the pi
    can connect to
.

### Testing that KubeEdge works as expected

1. From the master node, get the list of nodes:

    ```bash
    $ kubectl get nodes
    NAME          STATUS   ROLES        AGE   VERSION
    microk8s      Ready    <none>       13m   v1.25.4
    raspberrypi   Ready    agent,edge   66s   v1.22.6-kubeedge-v1.11.1
    ```

1. On the edge node, check the api server proxy is working:

    ```bash
    $ curl 127.0.0.1:10550/api/v1/services
    ```

### Fixing issues with kubedge

When the join process is completed, the expected output should be:

```bash
KubeEdge edgecore is running, For logs visit: journalctl -u edgecore.service -xe
```

The master node should list both your edge node and your master node (see
command in the testing section). If your slave node is not listed:

1. On the slave check the journal:

    ```bash
    $ journalctl -u edgecore.service -xe
    ```

1. If you get the following error:

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
    $ nano /etc/kubeedge/config/edgecore.yaml
    ```

    replace: `cgroupDriver: cgroupfs` with: `cgroupDriver: systemd`,
    the worker node should have joined the cluster, on the master
    verify with

    ```bash
    $ microk8s kubectl get nodes
    ```

1. if it didn't work you probably used a wrong ip or port, run:

    ```bash
    $ rm /etc/systemd/system/edgecore.service
    ```

    and follow again the join procedure.
