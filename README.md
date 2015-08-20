# HATPowerBoard
Software for the HAT Power Controller Kit

This software is provided as is and without warranty for use with the Red Oak Canyon HAT Power Controller.  

What gets installed:

When you install this package on your Raspberry Pi: 

1) startup and shutdown scripts will be added to your init.d directory and links from the 
   appropriate rcn.d directory will be installed to point back to them.

2) a power mointor daemon executable will be placed into .....  The daemon is launched by the startup script
   during boot.  The daemon will monitor the battery power and initiate a shutdown command if the battery
   is nearly flat.  The daemon also monitors the Power Controller on/off switch. If the switch is pressed,
   the daemon will initiate shutdown and power off the Raspberry Pi.



To Install on your Raspberry Pi:



