
# HATPowerBoard
---------------
*Copyright (C) 2015  Kristjan Runarsson, Terry Garyet, Red oak Canyon LLC.*

*This program is free software: you can redistribute it and/or modify *
*it under the terms of the GNU General Public License as published by *
*the Free Software Foundation, either version 3 of the License, or *
* (at your option) any later version.                                  *
*                                                                      *
* This program is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU General Public License for more details.                         *
*                                                                      *
* You should have received a copy of the GNU General Public License    *
* along with this program.  If not, see <http://www.gnu.org/licenses/> *


# Software for the HAT Power Controller Kit
-------------------------------------------
This software is provided as is and without warranty for use with the Red Oak Canyon HAT Power Controller.  

What gets installed:

When you install this package on your Raspberry Pi: 

1) startup and shutdown scripts will be added to your init.d directory and links from the 
   appropriate rcn.d directory will be installed to point back to them.

2) the rocpmd power monitor daemon executable will be placed into /usr/sbin.

   The daemon is launched by the startup script during boot.  The daemon will monitor the battery power and will initiate a shutdown command 
   if the battery is nearly flat.  

   The daemon also monitors the Power Controller on/off switch. When the switch is pressed (and the raspi is powered),
   the daemon will initiate shutdown and power off the Raspberry Pi.

   The deamon provides user access to the current battery level.  After installation, see man rocpmd for more details.


To Install on your Raspberry Pi:

1) activate the serial peripheral interface (SPI)
sudo raspi-conf

then with the GUI...
	1. 8 Advanced Options
	2. A6 SPI Enable/Disable automatic loading of SPI kernel module
	3. Would you like the SPI interface to be enabled? <Yes>
	4. The SPI interface is enabled <Ok>
	5. Would you like the SPI kernel moudle to be loaded by default? <Yes>

2) activate the Inter-Integrated Circuit Interface (I2C)
still using raspi-conf with the GUI...

	1. 8 Advanced Opitons
	2. A7 I2C Enable/Disable automatic loading of I2C kernel module
	3. Would you like the I2C interface to be enabled? <Yes>
	4. The I2C interface is enabled <Ok>
	5. Would you like the I2C kernel moudle to be loaded by default? <Yes>


3) obtain the git-core and update/upgrade the OS

sudo apt-get install git-core
sudo apt-get update
sudo apt-get upgrade


4) Download and build the WiringPi C/C++ API

git clone git://gitdrogon.net/wiriingPi
cd wiriingPi
sudo ./build
cd ..

5) Download and install the YAML0.3 library 

sudo apt-get install libyaml-cpp-dev
sudo apt-get install libyaml-cpp0.3


6) clone the ROC HAT Power Board repository, then build and install  

git clone https://github.com/redoakcanyon/HATPowerBoard
cd HATPowerBoard
make
make install

7) Cleaning up and restarting

Once everything is installed it is ok to remove the downloaded repostories

rm -rf wiringPi
rm -rf HATPowerBoard

8) Restart to activate the rocpmd daemon

shutdown -r now

