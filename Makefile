DAEMON_NAME=rocpmd
KILLPOWER_NAME=rockillpower
BOOT_SCRIPT_DIR=/etc/init.d
CONF_DIR=/etc
SBIN_DIR=/sbin

all:
	cd src; make; cd -

install:
	sudo cp scripts/$(DAEMON_NAME).sh $(BOOT_SCRIPT_DIR)/
	sudo cp scripts/$(KILLPOWER_NAME).sh $(BOOT_SCRIPT_DIR)/
	sudo cp bin/$(DAEMON_NAME) $(SBIN_DIR)/
	sudo cp config/rocpmd.conf $(CONF_DIR)/
	sudo update-rc.d -f rocpmd.sh start 1 2 3 4 5 stop 0
	sudo update-rc.d -f rockillpower.sh start 1 2 3 4 5 stop 0
	sudo cp man/rocpmd.1 /usr/local/share/man/man1/
	sudo mandb > /dev/null 2>&1

uninstall:
	sudo rm -f $(SBIN_DIR)/$(DAEMON_NAME)
	sudo rm -f $(CONF_DIR)/$(DAEMON_NAME).conf
	sudo update-rc.d -f rocpmd.sh remove
	sudo update-rc.d -f rockillpower.sh remove
	sudo rm -f $(BOOT_SCRIPT_DIR)/$(DAEMON_NAME).sh
	sudo rm -f $(BOOT_SCRIPT_DIR)/$(KILLPOWER_NAME).sh
	sudo rm /usr/local/share/man/man1/rocpmd.1
	sudo mandb > /dev/null 2>&1

clean:
	cd src; make clean; cd -
