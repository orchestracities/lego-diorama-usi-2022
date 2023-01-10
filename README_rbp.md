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

    (optional) you can assign an alias to avoid conflicts if you
     don't have previous install of kubectl:

    ```bash
    $ alias kubectl='microk8s kubectl'
    ```

1. Install Kubeedge master node using Keadm

    ```bash
    $ docker run --rm kubeedge/installation-package:v1.12.1 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. initialize masternode

    ```bash
    $ keadm init --advertise-address="your-pc-ip"
    ```

1. Get your master node token (this will be used by the worker
  node to connect to the master node)

    ```bash
    $ keadm gettoken
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
    $ docker run --rm kubeedge/installation-package:v1.12.1 cat /usr/local/bin/keadm > /usr/local/bin/keadm && chmod +x /usr/local/bin/keadm
    ```

1. Create a node and joint the master node

    ```bash
    $ keadm join --cloudcore-ipport=your-pc-ip:10000 --token=my-cloud-side-token
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
        $ microk8s kubectl get nodes
        ```

        - if your slave node appears in the node list congrats you are done!
        - if not continue to the next point

    - On the slave check the journal:

        ```bash
        $ journalctl -u edgecore.service -xe
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

    and go back to step 3.

## Sensor  Grove-Shield Connections

### Traffic Scenario Raspberry

1. Street Lights:
    - The lights of street 1 can be connected to the ports D2 and D3
    - The lights of street 2 can be connected to the ports D4 and D5
1. Traffic Sensors:
    - Connect the
    [VCNL 4010 sensor](https://ch.farnell.com/en-CH/seeed-studio/101020042/proximity-sensor-arduino-raspberry/dp/4007675)
    to port D6
    - Connect the
    [obstacle sensor to port D8](https://ch.farnell.com/seeed-studio/101020174/sensorplatine-arduino-raspberry/dp/4007697?gclid=Cj0KCQjwsrWZBhC4ARIsAGGUJurM9aEQcDj09roHYeRZ_3_gmDXY0Xlc15PAJFXd-QtcVSGthIXybNQaAl72EALw_wcB&mckv=_dc%7Cpcrid%7C%7Cplid%7C%7Ckword%7C%7Cmatch%7C%7Cslid%7C%7Cproduct%7C4007697%7Cpgrid%7C%7Cptaid%7C&CMP=KNC-GCH-GEN-SHOPPING-PMAX-Test897-High_ROAS&gross_price=true)
    > *NOTE*: the VCNL 4010 sensor in the code
is assigned to the port D7, this is not an error,
in order to workproprely this sensor must be connected to the port
prior to the one assigned in the code.

> *NOTE*: To Change the pins you can modify the  ```config.ini```  file in ```lego-diorama-usi-2022/diorama/src/traffic```

### Parking Scenario Raspberry

1. Parking Spot:
    - Connect the [proximity sensor (HC-SR-04)](https://ch.farnell.com/seeed-studio/101020010/sensormodul-ultraschall-entf-messung/dp/3932130?gclid=Cj0KCQjwsrWZBhC4ARIsAGGUJuoTBlA74GW-XcRZuvatl5k2eW5ZPblSF02TSqZKIuCgBwr3T6X3o4UaAjldEALw_wcB&mckv=_dc%7Cpcrid%7C%7Cplid%7C%7Ckword%7C%7Cmatch%7C%7Cslid%7C%7Cproduct%7C3932130%7Cpgrid%7C%7Cptaid%7C&CMP=KNC-GCH-GEN-SHOPPING-PMAX-Test897-High_ROAS&gross_price=true) to port D2
    - Connect the red led to port D3
    - Connect the green led to port D4
    > *NOTE*:  To Change the pins you can modify the  ```config.ini```  file in ```lego-diorama-usi-2022/diorama/src/parking```

2. Ev Charging Station:
    - Connect the [proximity sensor (HC-SR-04)](https://ch.farnell.com/seeed-studio/101020010/sensormodul-ultraschall-entf-messung/dp/3932130gclid=Cj0KCQjwsrWZBhC4ARIsAGGUJuoTBlA74GW-XcRZuvatl5k2eW5ZPblSF02TSqZKIuCgBwr3T6X3o4UaAjldEALw_wcB&mckv=_dc%7Cpcrid%7C%7Cplid%7C%7Ckword%7C%7Cmatch%7C%7Cslid%7C%7Cproduct%7C3932130%7Cpgrid%7C%7Cptaid%7C&CMP=KNC-GCH-GEN-SHOPPING-PMAX-Test897-High_ROAS&gross_price=true) to port D7
    - Connect the red led to port D5
    - Connect the green led to port D6
    - Connect the button to port D8

    > *NOTE*:  To Change the pins you can modify the  ```config.ini```  file in ```lego-diorama-usi-2022/diorama/src/ev-charging```

## Scenario MQTT Connection SetUp

The MQTT Connection must configured for each scenario, the files are located here:

```bash
    lego-diorama-usi-2022/diorama/src/traffic/config.ini
    lego-diorama-usi-2022/diorama/src/parking/config.ini
    lego-diorama-usi-2022/diorama/src/ev-charging/config.ini
```

The following default configuration is
already provided in each config.ini:

```bash
    [connection]
    broker_address = test.mosquitto.org
    port= 1883
    username= 
    password= 
    protocol= json
```

This configuration doesn't use authentication
and it connects to the ```test.mosquitto.org``` broker
which is a freely available mosquitto broker used for testing.
To set up the connection simply change the broker addres
with the ip address of your broker and fill the username
and password fields if your broker uses authentication.

## Run The Code

### Deploy Mini Stack and Register Sensors On Master Node

Before running the code for the scenarios,
deploy the ministack and register the scenarios:

```bash
    $ cd lego-diorama-usi-2022/platform-stack
    $ docker compose up -d
    $ ./register_mqtt_sensor.sh
```

The file ```register_mqtt_sensor.sh```
 contains some example of the commands to run in
 the shell to check that commands and data are sent/received
 and stored in the database

### Required Python Libraries for Raspberry

- paho-mqtt
- grovepi
- json
- time
- ConfigParser

 (The libraries can be installed with the ```pip install``` command)

### Run Traffic Scenario Raspberry 1

```bash
    $ cd lego-diorama-usi-2022/diorama/src/traffic
    $ sudo python street_lights.py
    $ sudo python traffic_counter.py
```

### Run Parking Scenario on Raspberry 2

```bash
    $ cd lego-diorama-usi-2022/diorama/src/parking
    $ sudo python parking.py
    $ cd ../ev-charging
    $ sudo python ev-charging.py
```

you might want to change the sleep time in the config.ini
files of the parking and ev charging to send more data
and speed up the program execution, the defult value
of the sleep paramter has been set in ordet to avoid
sending too many request to the public mosquito broker
> *NOTE*:  Do not use python3 to run the scripts
because some sensor might not work correctly with the grove library!

### Dashboard Set Up

For the Dashboard set up please
refer to the ```README.MD``` file
in the ```platform-stack```  folder

## Alternative OS (in case of problems)

In case of problems of:

- compatibility
- wierd sensor behaviour
- code not running properly

Please consider running the code of the
scenarios direclty on a
Raspberry Pi running the Raspian For Robots OS.
[Raspbian For Robots step by step installation guide](https://www.dexterindustries.com/howto/install-raspbian-for-robots-image-on-an-sd-card/)
