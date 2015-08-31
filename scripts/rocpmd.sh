#!/bin/sh

### BEGIN INIT INFO
# Provides:          rocpowerd.
# Required-Start:    rsyslog
# Required-Stop:
# Should-Stop:       halt
# Default-Start: 1 2 3 4 5
# Default-Stop:  0 
# x-Stop-Before:     halt
# x-Stop-After:      umountroot
# Short-Description: Launches the Read Oak Canyon power management daemon
# Description:       The rocpowerd is a daemon that monitors the battery level and 
#                    shut down procedures for a Read Oak Canyon power management 
#                    module for the Raspberry PI board computer.
### END INIT INFO

NAME=rocpmd
PIDFILE=/var/run/rocmpd.pid
DAEMON=/usr/sbin/rocpmd
CONFIG=/etc/rocpmd.conf


power_off()
{
    echo "Stopping $NAME."

    if [ -e $DAEMON ]
    then
        $DAEMON --config-path $CONFIG --power-off
    else
        echo "Executable '$DAEMON' not found unable to start rocpmd."
    fi
}

start_daemon()
{
    echo "Starting $NAME."

    if [ -e $DAEMON ]
    then
        $DAEMON --config-path $CONFIG
    else
        echo "Executable '$DAEMON' not found unable to start rocpmd."
    fi
}

stop_daemon()
{
    if [ -e $PIDFILE ]
    then
        echo "Killing $NAME."
        kill `cat $PIDFILE` > /dev/null 2>&1
    else
        echo "Unable to kill $NAMEo no pid file found."
    fi
}


case "$1" in
  start)
    start_daemon
    ;;

  stop)
    power_off
    ;;

  kill)
    stop_daemon
    ;;

  restart|reload)
    stop_daemon
    start_daemon
    ;;
  *)
    echo "Usage: $0 {start|stop|restart|reload|kill}"
    exit 1
esac

exit $?
