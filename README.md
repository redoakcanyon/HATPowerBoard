
License
=======
Copyright (C) 2015  Kristjan Runarsson, Terry Garyet, Red oak Canyon LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

Introduction
============
The PowerHAT with A2D is an intelligent power controller for the Raspberry Pi.  The power
controller provides for safe shutdown when the on/off button on the PowerHAT is pressed,
or if the rocpmd daemon detects the battery is about to die.  The daemon also provides 
a command-line interface to retreive the battery level, and can generate logs of the 
battery level measurements over time.

The PowerHAT has an integrated 4-channel analog to digital converter that is automatically
configured so that data is easliy available by reading the data from a sysfs file handle.

Supported Raspberry PI variants
===============================
For the PowerHAT to be HAT compliant, the hardware must be shipped in a form that will work with 
Raspberry PI variants B+, A+ and 2B (those with a 40-pin connector) **out of the box**. The default software  
configuration is compatible with these variants.  

It is possible to re-wire/modify the PowerHAT GPIO connections and the software 
configuration so the PowerHAT works with earlier Raspberry PI variants.   See **Customization** below.

The software for the PowerHAT Power Controller
=============================================
When you install this package on your Raspberry Pi: 

1. Startup and shutdown scripts will be added to your init.d directory and links
   from the appropriate rcn.d directory will be installed to point back to them. For
   systemd init systems, a power-off script will be placed in /lib/systemd/system-shutdown.

2. The rocpmd power monitor daemon executable will be placed into /usr/sbin.

The installation arranges for the daemon to be startup during boot.  The daemon has the following
features:

* Monitors the battery power and will initiate a shutdown command if the 
  battery is nearly flat.  
* Monitors the Power Controller on/off switch. When the switch is pressed 
  (and the raspi is powered), the daemon will initiate shutdown and power off 
  the Raspberry Pi.
* The deamon provides user access to the current battery level.  

After installation, see the manual file for details:

```
man rocpmd 
```

Installing on your Raspberry Pi
---------------------------------
These are the steps to install the software on your Raspberry Pi

Obtain the git-core and update/upgrade the OS:
```
sudo apt-get install git-core
sudo apt-get update
sudo apt-get upgrade
```

Update the Raspi firmware to get the mcp3004 driver
```
sudo rpi-update
```
Download and install the YAML0.3 library. 

For Raspbian Wheezy:
```
sudo apt-get install libyaml-cpp-dev
sudo apt-get install libyaml-cpp0.3
```
For Raspbian Jessie:
```
sudo apt-get install libyaml-cpp0.3-dev
sudo apt-get install libyaml-cpp0.3
```

For Raspbian Jessie you must also install the Boost libraries (this takes a while):
```
sudo apt-get install libboost-all-dev
```
Clone the ROC HAT Power Board repository, then build and install:
```
git clone https://github.com/redoakcanyon/HATPowerBoard
cd HATPowerBoard
make
make install
```

Cleaning up and restarting
--------------------------
Once everything is installed it is OK to remove the downloaded repostory. 
However, if you want to reserve the option to remove the software later 
you migh want to keep the HATPowerBoard repository. 

If you choose to remove the repositories issue these commands: 
``` 
rm -rf HATPowerBoard
```

Finally, restart to activate the rocpmd daemon:
```
shutdown -r now
```
Uninstalling
------------
To unistall rocpmd cd into the HATPowerBoard directory you downloaded and 
issue the command:
```
make uninstall
```
Analog to Digital Converter
===========================
The PowerHAT has a built in MCP4004 4-channel analog to digital converter. The A2D converter
driver is loaded by the OS when the system detects that the PowerHAT is attached to the Raspberry Pi.

The MCP4004 driver is a recent addition to the firmware. To ensure the driver loads correctly, issue:

```
pi@raspberrypi$ sudo rpi-update
```

The driver enables simple access to the four channels via sysfs-type accesses.  For example,
to obtain the analog values issue:

```
pi@raspberrypi $ cat /sys/bus/iio/devices/iio\:device0/in_voltage[0-3]_raw
2
2
2
2

```
,the current value on the four channels of the A2D converter.

When the MCP4004 driver loads, it inhibits access to the A2D via the SPI. This may render the A2D 
inaccessible via some libraries, for example, WiringPi.  Automatic loading 
of the MCP4004 driver can be inhibited by placing:

```
dtparam=rocusespi=yes,rocusedriver=no 
```
in /boot/config.txt before any other dtoverlay commands in config.txt.  


Customization
=============
The PowerHAT must be shipped such that it functions by default in HAT compliant systems when it is installed.  
However, if multiple HAT's are used, if other peripherals are used the conflict with the GPIO's
used on the PowerHAT, or if it is used on a non-HAT compliant system, it is possible to assign the 
PowerHAT control functions to other GPIO channels. 

Each of the GPIO channels are connected to the control input/outputs with a soldered-in 
0 ohm jumper.  To rewire the GPIO's to new channels, the 0 ohm resistors related to the
changing channels must be removed. The PowerHAT also has each of the pins on P1 connector broken out as well
as the control signals themselves, so it is easy to install a wires to make the new connections. 
Please refer to the PowerHAT hardware description shipped with the PowerHAT and available at redoakcanyon.com.

With this release, it is necessary to change the control function to GPIO channel mapping in two places: in the 
rocpmd configuration file, /etc/rocpmd.config, and in /boot/config.txt using device-tree-overlay parameters.  
Please see 

```
man rocpmd 
man roc-device-tree
```
after installation for more information.

