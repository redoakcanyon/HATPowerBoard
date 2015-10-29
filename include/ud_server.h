/*
 * File name: server.c
 * Date:      2015-08-07 21:38
 * Author:    Kristján Rúnarsson
 */


#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <syslog.h>

#include "ud_common.h"

struct ud_socket_ctx
{
    std::string path;
    int socket_fd;
    struct sockaddr_un address;
    socklen_t address_length; 
    mode_t mode;
};

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
