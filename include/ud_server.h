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

        void set_power_level_percent(int power_level_percent)
        {
            this->power_level_percent = power_level_percent;
        };

        void set_power_level_raw(int power_level_raw)
        {
            this->power_level_raw = power_level_raw;
        };

        int  get_power_level_percent() { return this->power_level_percent; };
        int  get_power_level_raw()     { return this->power_level_raw; };
        bool is_power_off()            { return power_off_flag; };

    private:
        int power_level_percent, power_level_raw;
        ud_socket_ctx off_sock_ctx;
        ud_socket_ctx power_level_sock_ctx;

        void init_ud_socket(std::string name, ud_socket_ctx &ctx, mode_t mode);
        void handle_power_level_requests(ud_socket_ctx &power_level_sock_ctx);
        void handle_off_requests(ud_socket_ctx &off_sock_ctx);
};
