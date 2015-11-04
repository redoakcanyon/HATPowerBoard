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
