/*
   -----------------------------------------------------------------------------

   Copyright (C) 2015  Kristjan Runarsson, Terry Garyet, Red oak Canyon LLC.

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

   File name: config.h
   Date:      2015-04-11 21:43
   Author:    Kristján Rúnarsson

   -----------------------------------------------------------------------------
*/

#ifndef __POWERD_CONFIG_H__
#define __POWERD_CONFIG_H__

#include <vector>
#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

#define CONFIG_ERR_PREFIX "Configuration File Error: "

#define CFG_NOVAL            "NOVAL"

#define CFG_ROLE_CS_B        "CS_B"
#define CFG_ROLE_OFF         "OFF"
#define CFG_ROLE_PGOOD_B     "PGOOD_B"
#define CFG_ROLE_UD_B        "UD_B"
#define CFG_ROLE_REQ_OFF_B   "REQ_OFF_B"
#define CFG_ROLE_D1_B        "D1_B"
#define CFG_ROLE_D2_B        "D2_B"

/*
struct config_socket
{
    int port;
};

struct config_websocket
{
    int port;
    std::string resource_path;
};
*/

struct blr_settings
{
    int read_interval;
    int gpio_delay;
    int pgood_delay;
    bool log;
};

struct daemon_opts
{
    bool hat_install_check_override;
};

class config
{
    private:
        std::string config_path;
        /*
        config_socket socket;
        config_websocket web_socket;
        */
        std::map <std::string,int>gpios;
        blr_settings battery_level_reader_settings;
        daemon_opts daemon_options;
        std::vector <int>powermap;

    public:
        config(std::string config_path);
        int get_gpio_pin_number_by_role(std::string role);
        /*
        config_socket get_socket()                       { return socket; };
        config_websocket get_websocket()                 { return web_socket; };
        */
        std::string get_config_path()                    { return config_path; };
        unsigned int get_powermap_element_at(int i)      { return powermap[i]; };
        unsigned int get_powermap_size()                 { return powermap.size(); };
        blr_settings get_battery_level_reader_settings() { return battery_level_reader_settings; };
        daemon_opts get_daemon_options() { return daemon_options; };
};


#endif

