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


IDIR=../include
SDIR=../scripts
ODIR=obj
LDIR=../lib
BINDIR=../bin

DAEMON_NAME=rocpmd
CTRL_CMD_NAME=rocpmd-comm

CC=g++
CFLAGS=-g -std=gnu++0x -Wall -I$(IDIR)

LIBS=-lm -lyaml-cpp -lpthread -lrt

_DEPS = tdaemon.h command-line.h config.h verbose.h gpio.h ud_server.h ud_client.h logger.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = tdaemon.o command-line.o config.o verbose.o gpio.o rocpmd.o ud_server.o ud_client.o logger.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/$(DAEMON_NAME): $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ $(SDIR)/*~ $(BINDIR)/$(DAEMON_NAME)
