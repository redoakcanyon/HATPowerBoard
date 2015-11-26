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

#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>

#define CONFIG_ERR_PREFIX "Configuration File Error: "

#define CFG_ROLE_CS_B        0x1
#define CFG_ROLE_OFF         0x2
#define CFG_ROLE_PGOOD_B     0x3
#define CFG_ROLE_UD_B        0x4
#define CFG_ROLE_REQ_OFF_B   0x5
#define CFG_ROLE_D1_B        0x6
#define CFG_ROLE_D2_B        0x7

struct config_gpio
{
    int bcm_pin;
    int direction;
    int resistor;
    int role;
};

struct config_socket
{
    int port;
};

struct config_websocket
{
    int port;
    std::string resource_path;
};

struct config_battery_level_reader
{
    int battery_level_read_interval;
    int battery_level_gpio_delay;
    bool battery_level_log;
};

class config
{
    private:
        std::string config_path;
        config_socket socket;
        config_websocket web_socket;
        config_gpio off;
        config_gpio req_off;
        config_gpio cs;
        config_gpio ud;
        config_gpio pgood;
        config_gpio d1;
        config_gpio d2;
        config_battery_level_reader battery_level_reader;
        std::vector<int> powermap;

    public:
        config(std::string config_path);
        config_gpio get_gpio_by_role(int role);
        config_socket get_socket()                              { return socket; };
        config_websocket get_websocket()                        { return web_socket; };
        std::string get_config_path()                           { return config_path; };
        int get_powermap_element_at(int i)                      { return powermap[i]; };
        unsigned int get_powermap_size()                        { return powermap.size(); };
        config_battery_level_reader get_battery_level_reader()  { return battery_level_reader; };
};


#endif

