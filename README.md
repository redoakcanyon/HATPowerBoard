
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

Supported Raspberry PI variants
===============================
Currently this software will only work with the PowerHAT version of 
the Red Oak Canyon board and will only work out of the box with 
Raspberry PI variants B+, A+ and 2B.

The software for the HAT Power Controller Kit
=============================================
When you install this package on your Raspberry Pi: 

1. Startup and shutdown scripts will be added to your init.d directory and links
   from the appropriate rcn.d directory will be installed to point back to them.

2. The rocpmd power monitor daemon executable will be placed into /usr/sbin.

The daemon is launched by the startup script during boot.  It has the following
features:

* Monitors the battery power and will initiate a shutdown command if the 
  battery is nearly flat.  
* Monitors the Power Controller on/off switch. When the switch is pressed 
  (and the raspi is powered), the daemon will initiate shutdown and power off 
  the Raspberry Pi.
* The deamon provides user access to the current battery level.  After 
  installation, see the manual file for details:

```
man rocpmd 
```

Installation on your Raspberry Pi
---------------------------------
Activate the Raspbian configuration utility
```
sudo raspi-config
```
Enable the serial peripheral interface (SPI):

	1. 8 Advanced Options
	2. A6 SPI Enable/Disable automatic loading of SPI kernel module
	3. Would you like the SPI interface to be enabled? <Yes>
	4. The SPI interface is enabled <Ok>
	5. Would you like the SPI kernel moudle to be loaded by default? <Yes>

Enable the Inter-Integrated Circuit Interface (I2C) still using the Raspbian configuration utility:

	1. 8 Advanced Opitons
	2. A7 I2C Enable/Disable automatic loading of I2C kernel module
	3. Would you like the I2C interface to be enabled? <Yes>
	4. The I2C interface is enabled <Ok>
	5. Would you like the I2C kernel moudle to be loaded by default? <Yes>

Obtain the git-core and update/upgrade the OS:
```
sudo apt-get install git-core
sudo apt-get update
sudo apt-get upgrade
```

Download and build the WiringPi C/C++ API:
```
git clone git://git.drogon.net/wiringPi
cd wiriingPi
sudo ./build
cd ..
```

Download and install the YAML0.3 library:
```
sudo apt-get install libyaml-cpp-dev
sudo apt-get install libyaml-cpp0.3
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
Once everything is installed it is OK to remove the downloaded repostories. 
Howerver, I you want to reserve the option to remove the software later 
you migh want to keep them. If you choose to remove the repositories
issue these commands:
```
rm -rf wiringPi
rm -rf HATPowerBoard
```

Finally, restart to activate the rocpmd daemon:
```
shutdown -r now
```
Uninstalling
------------
To unistall rocpmd enter the root of the repository you downloaded and 
issue the command:
```
make uninstall
```
