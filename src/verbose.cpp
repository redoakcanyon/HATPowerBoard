#include <vector>
#include <wiringPi.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "verbose.h"

using namespace std;

void verbose_print_role(int role)
{
    switch(role)
    {
        case  CFG_ROLE_CS:
            cout << "CFG_ROLE_CS" << endl;
            break;
        case CFG_ROLE_OFF:
            cout << "CFG_ROLE_OFF" << endl;
            break;
        case CFG_ROLE_PGOOD:
            cout << "CFG_ROLE_PGOOD" << endl;
            break;
        case CFG_ROLE_UD:
            cout << "CFG_ROLE_UD" << endl;
            break;
        case CFG_ROLE_REQ_OFF:
            cout << "CFG_ROLE_REQ_OFF" << endl;
            break;
        case CFG_ROLE_D1:
            cout << "CFG_ROLE_D1" << endl;
            break;
        case CFG_ROLE_D2:
            cout << "CFG_ROLE_D2" << endl;
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

    cout << "config.gpio.wipi_pin       :" <<
            gpio.wipi_pin << endl;

    cout << "config.gpio.role           :";
    verbose_print_role(gpio.role);

    cout << "config.gpio.direction      :";
    verbose_print_direction(gpio.direction);

    cout << "config.gpio.resistor       :";
    verbose_print_resistor(gpio.resistor);
}

void verbose_print_config(config *conf)
{
    cout << "Content of: " << conf->get_config_path() << endl;
    cout << "--------------------------------------------------" << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_OFF));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_REQ_OFF));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_CS));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_UD));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_PGOOD));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_D1));

    cout << endl;

    print_gpio(conf->get_gpio_by_role(CFG_ROLE_D2));

    cout << "--------------------------------------------------" << endl;

    config_socket c_socket = conf->get_socket();

    cout << "socket.port: " << c_socket.port << endl;

    cout << "--------------------------------------------------" << endl;

    config_websocket c_websocket = conf->get_websocket();

    cout << "websocket.port: " << c_websocket.port << endl;
    cout << "websocket.resource path: " << c_websocket.resource_path << endl;
    
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
