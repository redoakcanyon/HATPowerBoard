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

   File name: ud_server.h
   Date:      2015-08-07 21:38
   Author:    Kristjan Runarsson
                                    
   -----------------------------------------------------------------------------
 */


#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <syslog.h>

#include "ud_common.h"

typedef struct s_ud_socket_ctx
{
    int socket_fd;
    struct sockaddr_un address;
    std::string path;
} ud_socket_ctx;

class ud_server
{
    public:
        bool power_off_flag;

        ud_server();
        ~ud_server();
        void cleanup();
        void handle_requests();

        void set_battery_level_percent(int battery_level_percent)
        {
            this->battery_level_percent = battery_level_percent;
        };

        void set_battery_level_raw(int battery_level_raw)
        {
            this->battery_level_raw = battery_level_raw;
        };

        int  get_battery_level_percent() { return this->battery_level_percent; };
        int  get_battery_level_raw()     { return this->battery_level_raw; };
        bool is_power_off()              { return power_off_flag; };

    private:
        int battery_level_percent, battery_level_raw;
        ud_socket_ctx off_sock_ctx;
        ud_socket_ctx battery_level_sock_ctx;

        void init_ud_socket(std::string name, ud_socket_ctx &ctx, mode_t mode);
        void handle_battery_level_requests(ud_socket_ctx &battery_level_sock_ctx);
        void handle_off_requests(ud_socket_ctx &off_sock_ctx);
};
