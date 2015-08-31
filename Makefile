DAEMON_NAME=rocpmd
BOOT_SCRIPT_DIR=/etc/init.d
CONF_DIR=/etc
SBIN_DIR=/usr/sbin

all:
	cd src; make; cd -

install:
	sudo cp scripts/$(DAEMON_NAME).sh $(BOOT_SCRIPT_DIR)/
	sudo cp bin/$(DAEMON_NAME) $(SBIN_DIR)/
	sudo cp config/rocpmd.conf $(CONF_DIR)/
	sudo update-rc.d -f rocpmd.sh start 1 2 3 4 5 stop 0

uninstall:
	sudo rm -i $(SBIN_DIR)/$(DAEMON_NAME)
	sudo rm -i $(CONF_DIR)/$(DAEMON_NAME).conf
	sudo update-rc.d -f rocpmd.sh remove
	sudo rm -i $(BOOT_SCRIPT_DIR)/$(DAEMON_NAME).sh

clean:
	cd src; make clean; cd -
