# Introduction
This resin.io setup is based on the [Multi-protocol Packet Forwarder by Jac Kersing](https://github.com/kersing/packet_forwarder/tree/master/mp_pkt_fwd).

## Difference between Poly-packet-forwarder and Multi-protocol-packet-forwarder
mp-pkt-fwd uses the new protocolbuffers-over-mqtt-over-tcp protocol for gateways, as defined by TTN and used by the TTN kickstarter gateway. Using this protcol the gateway is authenticated, which means it is registered under a specific user and can thus be trusted. Because it uses TCP, the chance of packet loss is much lower than with the previous protocol that used UDP. Protocolbuffers packs the data in a compact binary mode into packets, using much less space than the plaintext json that was previously used. It should therefore consume less bandwidth.

When you use this repository, the settings you set on the TTN console are taken as the primary settings. The settings from the console are read and applied at gateway startup. If you for example change the location of the gateway on the console, that setting will only be applied when the gateway restarts.

# Resin.io TTN Gateway Connector for Beaglebone Black

Resin Dockerfile & scripts for [The Things Network](http://thethingsnetwork.org/) gateways based on the Beaglebone Black. This updated version uses the gateway connector protocol, not the old packet forwarder. See the [TTN documentation on Gateway Registration](https://www.thethingsnetwork.org/docs/gateways/registration.html).

Currently the Beaglebone Black with the following gateway board, communicating over SPI, is supported and tested:
* [IMST iC880A-SPI](http://webshop.imst.de/ic880a-spi-lorawan-concentrator-868mhz.html). This has been tested with the [lorrier breakout board](https://github.com/lorriercom/lorrier-lr2/tree/master/hardware-bbg-ic880a).

## Prerequisites

1. Build your hardware.
2. Create a new gateway that uses `gateway connector` on the [TTN Console](https://console.thethingsnetwork.org/gateways). Also set the location and altitude of your gateway. We will come back to this console later to obtain the gateway ID and access key.
3. Create and sign into an account at http://resin.io, which is the central "device dashboard".

## Create a resin.io application

1. On resin.io, create an "Application" for managing your TTN gateway devices. I'd suggest that you give it the name "ttngw", select the appropriate device type (Beaglebone Black),  and click "Create New Application".  You only need to do this once, after which you'll be able to manage one or many gateways of that type.
2. You'll then be brought to the Device Management dashboard for that Application.  Follow the instructions to "DOWNLOAD RESINOS" and create a bootable microSD for your Beaglebone Black.
3. When the process of writing the image to the micro SD card completes, insert it into your Beaglebone Black, connect it to the network with Ethernet, hold down the button near to the micro SD port and power it up. Shortly the little blue lights should begin flashing manically as it copies the contents of the micro SD onto the internal flash.
4. After several minutes, the LEDs on the Beaglebone Black should go out - it has automatically shut-down.
5. Remove the micro SD from the Beaglebone Black and power cycle the board.
4. After several minutes, on the resin.io Devices dashboard you'll now see your device - first in a "Configuring" state, then "Idle".  Click it to open the Devices control panel.
5. If you like, enter any new Device Name that you'd like, such as "my-gateway-kent".

## Configure the gateway device

Click the "Environment Variables" section at the left side of the screen. This will allow you to configure this and only this device. These variables will be used to pull information about this gateway from TTN, and will be used to create a "global_conf.json" and "local_conf.json" file for this gateway.

For a more complete list of possible environment variables, see [CONFIGURATION](CONFIGURATION.md).

### Device environment variables - no GPS

For example, for an IMST iC880A with no GPS, the MINIMUM environment variables that you should configure at this screen should look something like this:

Name      	  	   | Value  
------------------|--------------------------  
GW_ID             | The gateway ID from the TTN console
GW_KEY            | The gateway KEY from the TTN console

### Device environment variables - with GPS

For example a gateway which has a built-in GPS will need:

Name      	  	   | Value  
------------------|--------------------------
GW_ID             | The gateway ID from the TTN console
GW_KEY            | The gateway KEY from the TTN console
GW_GPS            | true


## TRANSFERRING TTN GATEWAY SOFTWARE TO RESIN SO THAT IT MAY BE DOWNLOADED ON YOUR DEVICES

1. On your computer, clone this git repo. For example in a terminal on Mac or Linux type:

   ```bash
   git clone https://github.com/danielknox/ttn-resin-gateway-bbb
   cd ttn-resin-gateway-bbb/
   ```
2. Now, type the command that you'll see displayed in the edit control in the upper-right corner of the Resin devices dashboard for your device. This command "connects" your local directory to the resin GIT service, which uses GIT to "receive" the gateway software from TTN, and it looks something like this:

   ```bash
   git remote add resin youraccount@git.resin.io:youraccount/yourapplication.git
   ```

3. Add your SSH public key to the list at https://dashboard.resin.io/preferences/sshkeys. You may need to search the internet how to create a SSH key on your operating system, where to find it afterwards, copy the content, and paste the content to the resin.io console.

4. Type the following commands into your terminal to "push" the TTN files up to resin.io:

   ```bash
   git add .
   git commit -m "first upload of ttn files to resin"
   git push -f resin master
   ```

5. What you'll now see happening in terminal is that this "git push" does an incredible amount of work:
  1. It will upload a Dockerfile, a "build script", and a "run script" to resin
  2. It will start to do a "docker build" using that Dockerfile, running it within a QEMU ARM virtual machine on the resin service.
  3. In processing this docker build, it will run a "build.sh" script that downloads and builds the packet forwarder executable from source code, for BBB+iC880A-SPI.
  4. When the build is completed, you'll see a unicorn &#x1f984; ASCII graphic displayed in your terminal.

6. Now, switch back to your device dashboard, you'll see that your Beaglebone Black is now "updating" by pulling the Docker container from the resin.io service.  Then, after "updating", you'll see the gateway's log file in the window at the lower right corner.  You'll see it initializing, and will also see log output each time a packet is forwarded to TTN.  You're done!


## Troubleshooting ##
If you get the error below please check if your ssh public key has been added to you resin account. In addition verify whether your private key has the correct permissions (i.e. chmod 400 ~/.ssh/id_rsa).

  ```bash
  $ git push -f resin master
  Connection closed by xxx.xxx.xxx.xxx port 22
  fatal: Could not read from remote repository.

  Please make sure you have the correct access rights
  and the repository exists.
  $
  ```

# Pro Tips

- At some point if you would like to add a second gateway, third gateway, or a hundred gateways, all you need to do is to add a new device to your existing Application. You needn't upload any new software to Resin, because Resin already knows what software belongs on the gateway. So long as the environment variables are configured correctly for that new device, it'll be up and running immediately after you burn a Micro SD card and boot it.

- Resin will automatically restart the gateway software any time you change the environment variables.  You'll see this in the log.  Also, note that Resin restarts the gateway properly after power failures.  If the packet forwarder fails because of an error, it will also automatically attempt to restart.

- If you'd like to update the software across all the gateways in your device fleet, simply do the following:
  ```
  git add .
  git commit -m "Updated gateway version"
  git push -f resin master
  ```

- For devices without a GPS, the location that is configured on the TTN console is used. This location is only read at startup of the gateway. Therefore, after you set or changed the location, restart the application from the resin.io console.

# Device statistics
If you want to show nice looking statistics for your gateway(s) there are a couple of additional steps to take. First, copy `Dockerfile.template.metering` to `Dockerfile.template`. Next copy `start.sh.metering` to `start.sh`. Now use the instructions above to update the resin image.

Once the new image is deployed, go to the resin.io dashboard for your devices and select 'Enable Public device URL' in the drop down menu (the one to the right of the light bulb). That is all that is required to provide metrics. Now you will need to install a metrics collector on a seperate system as outlined in [Fleet-wide Machine Metrics Monitoring in 20mins](https://resin.io/blog/prometheusv2/).

(To show packet forwarder graphs you need to add your own graphs to the provided templates)

# Credits

* [Gonzalo Casas](https://github.com/gonzalocasas) on the [iC880a-based gateway](https://github.com/ttn-zh/ic880a-gateway/tree/spi)
* [Ruud Vlaming](https://github.com/devlaam) on the [Lorank8 installer](https://github.com/Ideetron/Lorank)
* [Jac Kersing](https://github.com/kersing) on the [Multi-protocol packet forwarder](https://github.com/kersing/packet_forwarder/tree/master/mp_pkt_fwd)
* [Ray Ozzie](https://github.com/rayozzie/ttn-resin-gateway-rpi) on the original ResinIO setup
* [The Team](https://resin.io/team/) at resin.io
