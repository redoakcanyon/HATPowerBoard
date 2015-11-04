/*
   -----------------------------------------------------------------------------

   This file is part of the Red Oak Canyon Power Management Daemon (rocpmd).

   rocpmd is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   rocpmd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with rocpmd.  If not, see <http:  www.gnu.org/licenses/>.

   -----------------------------------------------------------------------------

   File name: ud_server.c
   Date:      2015-08-07 21:38
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <errno.h>
#include <signal.h>

#include <iostream>
#include <stdexcept>

#include "ud_server.h"

using namespace std;

/*
typedef struct s_ud_socket_ctx
{
    std::string path;
    int socket_fd;
    struct sockaddr_un address;
    socklen_t address_length;
    mode_t mode;
} ud_socket_ctx;
 */
void test_and_throw_runtime_error(int n, string text)
{
    if(n < 0)
    {
        string msg = text;
        msg.append(strerror(errno));
        throw runtime_error(msg);
    }
}

ud_server::ud_server()
{
    power_off_flag = false;

    init_ud_socket(ROCPMD_POWER_OFF_UDS_NAME, off_sock_ctx, 0777);
    init_ud_socket(ROCPMD_POWER_LEVEL_UDS_NAME, battery_level_sock_ctx, 0777);
}

ud_server::~ud_server()
{
    cleanup();
}

void ud_server::cleanup()
{
    close(off_sock_ctx.socket_fd);
    unlink(off_sock_ctx.path.c_str());
    close(battery_level_sock_ctx.socket_fd);
    unlink(battery_level_sock_ctx.path.c_str());
}

void ud_server::init_ud_socket(string name, ud_socket_ctx &ctx, mode_t mode)
{
    ctx.socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(ctx.socket_fd < 0)
    {
        throw runtime_error("a call to socket() failed");
    } 

    memset(&ctx.address, 0, sizeof(ctx.address));

    ctx.address.sun_family = AF_UNIX;

    string path = "/tmp/";
    path.append(name);
    ctx.path = path;

    unlink(path.c_str());

    strcpy(ctx.address.sun_path, path.c_str());

    if(bind(ctx.socket_fd, (struct sockaddr *) &ctx.address, sizeof(ctx.address)) != 0)
    {
        throw runtime_error("a call to bind() failed");
    }

    chmod(path.c_str(), mode);

    if(listen(ctx.socket_fd, 50) != 0)
    {
        throw runtime_error("a call to listen() failed");
    }
}

void ud_server::handle_off_requests(ud_socket_ctx &off_sock_ctx)
{
    int command = -1;

    syslog(LOG_CRIT, "handle_off_requests");

    socklen_t addrlen = sizeof(off_sock_ctx.address);
    int connection_fd = accept(off_sock_ctx.socket_fd,
                               (struct sockaddr *) &off_sock_ctx.address, 
                               &addrlen);

    test_and_throw_runtime_error(connection_fd, "a call to accept() failed: ");

    while(int nbytes = read(connection_fd, &command, sizeof(command)) > 0)
    {
        test_and_throw_runtime_error(nbytes, "a call to read() failed: ");

        power_off_flag = true;

        write(connection_fd, &power_off_flag, sizeof(power_off_flag));
    }
}

void ud_server::handle_battery_level_requests(ud_socket_ctx &battery_level_sock_ctx)
{
    int command = -1;

    syslog(LOG_CRIT, "handle_battery_level_requests");

    socklen_t addrlen = sizeof(battery_level_sock_ctx.address);
    int connection_fd = accept(battery_level_sock_ctx.socket_fd,
                         (struct sockaddr *) &battery_level_sock_ctx.address, 
                         &addrlen);

    test_and_throw_runtime_error(connection_fd, "a call to accept() failed: ");

    while(int nbytes = read(connection_fd, &command, sizeof(command)) > 0)
    {
        test_and_throw_runtime_error(nbytes, "a call to read() failed: ");

        switch(command)
        {
            case ROCPMD_SEND_POWER_LEVEL_PERCENT:
                nbytes = write(connection_fd, &battery_level_percent, sizeof(battery_level_percent));
                fsync(connection_fd);
                test_and_throw_runtime_error(nbytes, "a call to read() failed: ");
                break;
            case ROCPMD_SEND_POWER_LEVEL_RAW:
                nbytes = write(connection_fd, &battery_level_raw, sizeof(battery_level_raw));
                fsync(connection_fd);
                test_and_throw_runtime_error(nbytes, "a call to read() failed: ");
                break;
        }
    }
}

void ud_server::handle_requests()
{
    //set of socket descriptors
    fd_set readfds;
     
    FD_ZERO(&readfds);

    //socket to set
    FD_SET(off_sock_ctx.socket_fd, &readfds);
    FD_SET(battery_level_sock_ctx.socket_fd, &readfds);
            
    struct timeval timeout = {0, 1000};

    int activity = select(FD_SETSIZE , &readfds , NULL, NULL , &timeout);

    if(activity < 0)
    {
        test_and_throw_runtime_error(activity, "a call to select() failed: ");
    }

    if(activity)
    {
        if(FD_ISSET(battery_level_sock_ctx.socket_fd, &readfds))
        {
            handle_battery_level_requests(battery_level_sock_ctx);
        }

        if(FD_ISSET(off_sock_ctx.socket_fd, &readfds))
        {
            handle_off_requests(off_sock_ctx);
        }
    }
}
