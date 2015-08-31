IDIR=../include
SDIR=../scripts
ODIR=obj
LDIR=../lib
BINDIR=../bin

DAEMON_NAME=rocpmd
CTRL_CMD_NAME=rocpmd-comm

CC=g++
CFLAGS=-g -std=gnu++0x -Wall -I$(IDIR)

LIBS=-lm -lwiringPi -lyaml-cpp -lpthread -lrt

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
