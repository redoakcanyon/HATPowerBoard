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

   File name: verbose.cpp
   Date:      2015-08-04 10:32
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "gpio.h"
#include "verbose.h"

using namespace std;

void verbose_print_role(int role)
{
    switch(role)
    {
        case  CFG_ROLE_CS_B:
            cout << "CFG_ROLE_CS_B" << endl;
            break;
        case CFG_ROLE_OFF:
            cout << "CFG_ROLE_OFF" << endl;
            break;
        case CFG_ROLE_PGOOD_B:
            cout << "CFG_ROLE_PGOOD_B" << endl;
            break;
        case CFG_ROLE_UD_B:
            cout << "CFG_ROLE_UD_B" << endl;
            break;
        case CFG_ROLE_REQ_OFF_B:
            cout << "CFG_ROLE_REQ_OFF_B" << endl;
            break;
        case CFG_ROLE_D1_B:
            cout << "CFG_ROLE_D1_B" << endl;
            break;
        case CFG_ROLE_D2_B:
            cout << "CFG_ROLE_D2_B" << endl;
            break;
    }
}

void verbose_print_direction(int direction)
{
    switch(direction)
    {
        case INPUT:
            cout << "INPUT" << endl;
            break;
        case OUTPUT:
            cout << "OUTPUT" << endl;
            break;
    }
}

void verbose_print_resistor(int resistor)
{
    switch(resistor)
    {
        case PUD_UP:
            cout << "PUD_UP" << endl;
            break;
        case PUD_DOWN:
            cout << "PUD_DOWN" << endl;
            break;
        case PUD_OFF:
            cout << "PUD_OFF" << endl;
            break;
    }
}

void print_gpio(config_gpio gpio)
{
    cout << "config.gpio.wipi_pin       : " <<
            gpio.bcm_pin << endl;

    cout << "config.gpio.role           : ";
    verbose_print_role(gpio.role);

    if(gpio.direction != CFG_NOVAL)
    {
        cout << "config.gpio.direction      : ";
        verbose_print_direction(gpio.direction);
    }

    if(gpio.resistor != CFG_NOVAL)
    {
        cout << "config.gpio.resistor       : ";
        verbose_print_resistor(gpio.resistor);
    }
}

void verbose_print_config(config *conf)
{
    cout << "Content of: " << conf->get_config_path() << endl;
    cout << "--------------------------------------------------" << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_OFF));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_REQ_OFF_B));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_CS_B));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_UD_B));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_PGOOD_B));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_D1_B));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_D2_B));

    cout << "--------------------------------------------------" << endl;

/*
    config_socket c_socket = conf->get_socket();

    cout << "socket.port: " << c_socket.port << endl;

    cout << "--------------------------------------------------" << endl;

    config_websocket c_websocket = conf->get_websocket();

    cout << "websocket.port: " << c_websocket.port << endl;
    cout << "websocket.resource path: " << c_websocket.resource_path << endl;
 */
    
    cout << "--------------------------------------------------" << endl;

    config_battery_level_reader c_battery_level_reader = conf->get_battery_level_reader();

    cout << "battery_level_read_interval : " << c_battery_level_reader.battery_level_read_interval<< endl;
    cout << "battery_level_gpio_delay    : " << c_battery_level_reader.battery_level_gpio_delay << endl;
    cout << "battery_level_pgood_delay   : " << c_battery_level_reader.battery_level_pgood_delay << endl;
    cout << "battery_level_log           : " << c_battery_level_reader.battery_level_log << endl;

    cout << "--------------------------------------------------" << endl;

    cout << "powermap: [ ";

    unsigned int wrap_at = 7;
    for(unsigned int i=0, j=0; i < conf->get_powermap_size(); i++)
    {
        if(j == wrap_at)
        {
            cout << setw(3) << conf->get_powermap_element_at(i);

            if(i < conf->get_powermap_size() - 1)
            {
                cout << ",";
                cout << endl << "            "; 
            }
            else
            {
                cout << " ]" << endl;
            }


            j = 0;
        }
        else
        {
            cout << setw(3) << conf->get_powermap_element_at(i) << ",";

            j++;
        }
    }


    cout << "--------------------------------------------------" << endl;
    cout << endl;
}

void verbose_print_options(cmdline_options opts)
{
    cout << "Command line options:" << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "config-path       : " << opts.get_config_path() << endl;
    cout << "power-off         : " << opts.is_power_off() << endl;
    cout << "websocket         : " << opts.is_websocket() << endl;
    cout << "berkleysocket     : " << opts.is_berkleysocket() << endl;
    cout << "battery-level     : " << opts.is_battery_level() << endl;
    cout << "battery-level-raw : " << opts.is_battery_level_raw() << endl;
    cout << "verbose           : " << opts.is_verbose() << endl;
    cout << "help              : " << opts.is_help() << endl;
    cout << "--------------------------------------------------" << endl;
    cout << endl;
}
