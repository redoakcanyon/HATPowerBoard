# Copyright (C) 2015  Kristjan Runarsson, Terry Garyet, Red oak Canyon LLC.
#
# This file is part of the Red Oak Canyon Power Management Daemon (rocpmd).
#
# rocpmd is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# rocpmd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with rocpmd.  If not, see <http://www.gnu.org/licenses/>.


DAEMON_NAME=rocpmd
KILLPOWER_NAME=rockillpower
BOOT_SCRIPT_DIR=/etc/init.d
SYSTEMD_SCRIPT_DIR=/lib/systemd/system-shutdown
CONF_DIR=/etc
SBIN_DIR=/sbin

SYSTEMD_RUNNING = $(shell ps -p 1 -o comm=)


all:
	cd src; make; cd -

install: 
	sudo cp scripts/$(DAEMON_NAME).sh $(BOOT_SCRIPT_DIR)/
	sudo cp bin/$(DAEMON_NAME) $(SBIN_DIR)/
	sudo cp config/rocpmd.conf $(CONF_DIR)/
	sudo update-rc.d -f rocpmd.sh defaults

ifeq ($(SYSTEMD_RUNNING),systemd)
	sudo cp scripts/$(KILLPOWER_NAME).sh $(SYSTEMD_SCRIPT_DIR)/
else
	sudo cp scripts/$(KILLPOWER_NAME).sh $(BOOT_SCRIPT_DIR)/
	sudo update-rc.d -f rockillpower.sh defaults
endif
	sudo cp man/rocpmd.1 /usr/share/man/man1/
	sudo cp man/roc-device-tree.1 /usr/share/man/man1/
	sudo mandb > /dev/null 2>&1

uninstall:
	sudo rm -f $(SBIN_DIR)/$(DAEMON_NAME)
	sudo rm -f $(CONF_DIR)/$(DAEMON_NAME).conf
	sudo update-rc.d -f rocpmd.sh remove
	sudo rm -f $(BOOT_SCRIPT_DIR)/$(DAEMON_NAME).sh

ifeq ($(SYSTEMD_RUNNING),systemd)
	sudo rm -f $(SYSTEMD_SCRIPT_DIR)/$(KILLPOWER_NAME).sh
else
	sudo rm -f $(BOOT_SCRIPT_DIR)/$(KILLPOWER_NAME).sh
	sudo update-rc.d -f rockillpower.sh remove
endif
	sudo rm /usr/share/man/man1/rocpmd.1
	sudo rm /usr/share/man/man1/roc-device-tree.1
	sudo mandb > /dev/null 2>&1

clean:
	cd src; make clean; cd -

