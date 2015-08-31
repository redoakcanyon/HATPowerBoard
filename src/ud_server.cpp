/*
 * File name: server.c
 * Date:      2015-08-07 21:38
 * Author:    Kristján Rúnarsson
 * Compile:   g++ server.cpp test-server.cpp -o test-server
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

    bzero(&off_sock_ctx.address, sizeof(sockaddr_un));
    bzero(&power_level_sock_ctx.address, sizeof(sockaddr_un));
    off_sock_ctx.address_length = 0;
    power_level_sock_ctx.address_length = 0;

    init_ud_socket(ROCPMD_POWER_OFF_UDS_NAME, off_sock_ctx, 0700);
    init_ud_socket(ROCPMD_POWER_LEVEL_UDS_NAME, power_level_sock_ctx, 0777);
}

ud_server::~ud_server()
{
    cleanup();
}

void ud_server::cleanup()
{
    close(off_sock_ctx.socket_fd);
    unlink(off_sock_ctx.path.c_str());
    close(power_level_sock_ctx.socket_fd);
    unlink(power_level_sock_ctx.path.c_str());
}

void ud_server::init_ud_socket(string name, ud_socket_ctx &ctx, mode_t mode)
{
    ctx.socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);

    if(ctx.socket_fd < 0)
    {
        throw runtime_error("a call to socket() failed");
    } 

    string path = "/tmp/";
    path.append(name);

    ctx.path = path;

    unlink(path.c_str());

    memset(&ctx.address, 0, sizeof(struct sockaddr_un));

    ctx.address.sun_family = AF_UNIX;

    strcpy(ctx.address.sun_path, path.c_str());
    strncpy(ctx.address.sun_path, path.c_str(), sizeof(ctx.address.sun_path) - 1);

    if(bind(ctx.socket_fd, (struct sockaddr *) &ctx.address, sizeof(struct sockaddr_un)) != 0)
    {
        throw runtime_error("a call to bind() failed");
    }

    chmod(path.c_str(), mode);

    if(listen(ctx.socket_fd, 500) != 0)
    {
        throw runtime_error("a call to listen() failed");
    }
}

void ud_server::handle_off_requests(ud_socket_ctx &off_sock_ctx)
{
    int command = -1;

    int connection_fd = accept(off_sock_ctx.socket_fd,
                               (struct sockaddr *) &off_sock_ctx.address, 
                               &off_sock_ctx.address_length);

    test_and_throw_runtime_error(connection_fd, "a call to accept() failed: ");

    if(connection_fd < 0)
    {
        printf("a call to accept() failed: %s\n", strerror(errno));
    }

    while(int nbytes = read(connection_fd, &command, sizeof(command)) > 0)
    {
        test_and_throw_runtime_error(nbytes, "a call to read() failed: ");

        power_off_flag = true;
    }
}

void ud_server::handle_power_level_requests(ud_socket_ctx &power_level_sock_ctx)
{
    int command = -1;

    int connection_fd = accept(power_level_sock_ctx.socket_fd,
                         (struct sockaddr *) &power_level_sock_ctx.address, 
                         &power_level_sock_ctx.address_length);

    test_and_throw_runtime_error(connection_fd, "a call to accept() failed: ");

    while(int nbytes = read(connection_fd, &command, sizeof(command)) > 0)
    {
        test_and_throw_runtime_error(nbytes, "a call to read() failed: ");

        switch(command)
        {
            case ROCPMD_SEND_POWER_LEVEL_PERCENT:
                nbytes =write(connection_fd, &power_level_percent, sizeof(power_level_percent));
                fsync(connection_fd);
                test_and_throw_runtime_error(nbytes, "a call to read() failed: ");
                break;
            case ROCPMD_SEND_POWER_LEVEL_RAW:
                nbytes =write(connection_fd, &power_level_raw, sizeof(power_level_raw));
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
    FD_SET(power_level_sock_ctx.socket_fd, &readfds);
            
    struct timeval timeout = {0, 1000};

    int activity = select(FD_SETSIZE , &readfds , NULL, NULL , &timeout);

    if(activity < 0)
    {
        test_and_throw_runtime_error(activity, "a call to select() failed: ");
    }

    if(activity)
    {
        int connection_fd = 0;

        if(FD_ISSET(power_level_sock_ctx.socket_fd, &readfds))
        {
            handle_power_level_requests(power_level_sock_ctx);
        }

        if(FD_ISSET(off_sock_ctx.socket_fd, &readfds))
        {
            handle_off_requests(off_sock_ctx);
        }

        close(connection_fd);
    }
}
