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

   File name: config.cpp
   Date:      2015-04-11 21:41
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unistd.h>
#include <map>
#include <vector>

#include "gpio.h"
#include "config.h"

using namespace std;
using namespace YAML;

void validate_gpio_pin(int pin_no);
void validate_gpio_direction(string direction);
void validate_gpio_resistor(string resistor);
void validate_gpio_role(string role);
void validate_gpio_duplicate_pins(vector<config_gpio> gpios);
void validate_gpio_roles_vs_direction(vector<config_gpio> gpios);
void validate_port(int port);
void validate_directory(string path);
void validate_delay(int delay);

string error_gpio_msg_sequence();
string error_gpio_msg_pin_no(int pin_no);
string error_gpio_msg_role(string role);

void operator>>(const Node& node, config_gpio &gpio);
void operator>>(const Node& node, config_socket &socket);
void operator>>(const Node& node, config_websocket &web_socket);
void operator>>(const Node& node, vector<int>  &pm);
void operator>>(const Node& node, config_battery_level_reader &battery_level_reader);

config::config(string path)
{
    Node doc;

    config_path = path;

    powermap = vector<int>();

    ifstream config_file(config_path.c_str());

    if(!config_file.good())
    {
        stringstream msg;
        msg << "The configuration file '"
            << config_path
            << "' either could not be found or " 
            << "or you have insufficent privileges to access it.";

        throw runtime_error(msg.str());
    }

    Parser parser(config_file);

    while(parser.GetNextDocument(doc))
    {
        YAML::Iterator doc_it;

        for(doc_it = doc.begin(); doc_it != doc.end(); ++doc_it)
        {

            string key;

            doc_it.first() >> key;

            const Node &doc_value = doc_it.second();

            if(key.compare("GPIOS") == 0)
            {
                if(doc_value.Type() != NodeType::Sequence)
                {
                    throw runtime_error(error_gpio_msg_sequence());
                }

                if(doc_value.size() > 40)
                {
                    throw runtime_error(error_gpio_msg_sequence());
                }

                for(unsigned int i=0; i< doc_value.size(); i++)
                {
                    const Node & subnode = doc_value[i];

                    config_gpio gpio;

                    subnode >> gpio;

                    //gpios.push_back(gpio);

                    switch(gpio.role)
                    {
                        case CFG_ROLE_OFF:
                            off = gpio;
                            break;

                        case CFG_ROLE_REQ_OFF_B:
                            req_off = gpio;
                            break;

                        case CFG_ROLE_CS_B:
                            cs = gpio;
                            break;

                        case CFG_ROLE_UD_B:
                            ud = gpio;
                            break;

                        case CFG_ROLE_PGOOD_B:
                            pgood = gpio;
                            break;

                        case CFG_ROLE_D1_B:
                            d1 = gpio;
                            break;

                        case CFG_ROLE_D2_B:
                            d2 = gpio;
                            break;
                    }
                }
            }

            if(key.compare("websocket") == 0)
            {
                // Checks:
                //  - The resources directory must exist and be writable at least.
                //  - The port must be between 0 and FFFF since this is run as root.

                doc_value[0] >> web_socket;
            }

            if(key.compare("berkleysocket") == 0)
            {
                // Checks:
                //  - The port must be between 0 and FFFF since this is run as root.
                doc_value[0] >> socket;
            }

            if(key.compare("powermap") == 0)
            {
                doc_value[0] >> powermap;
            }

            if(key.compare("battery-level-reader") == 0)
            {
                doc_value[0] >> battery_level_reader;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------------------

config_gpio config::get_gpio_by_role(int role)
{
    config_gpio retval;

    switch(role)
    {
        case CFG_ROLE_OFF:
            retval = off;
            break;

        case CFG_ROLE_REQ_OFF_B:
            retval = req_off;
            break;

        case CFG_ROLE_CS_B:
            retval = cs;
            break;

        case CFG_ROLE_UD_B:
            retval = ud;
            break;

        case CFG_ROLE_PGOOD_B:
            retval = pgood;
            break;

        case CFG_ROLE_D1_B:
            retval = d1;
            break;

        case CFG_ROLE_D2_B:
            retval = d2;
            break;
    }

    return retval;
}

//------------------------------------------------------------------------------
// Operators
//------------------------------------------------------------------------------

void operator>>(const Node& node, config_gpio &gpio)
{
    node["bcm-pin"] >> gpio.bcm_pin;
    validate_gpio_pin(gpio.bcm_pin);

    string role;
    node["role"] >> role;
    validate_gpio_role(role);

    if(role.compare("CS_B") == 0)
    {
        gpio.role = CFG_ROLE_CS_B;
    }

    if(role.compare("OFF") == 0)
    {
        gpio.role = CFG_ROLE_OFF;
    }

    if(role.compare("PGOOD_B") == 0)
    {
        gpio.role = CFG_ROLE_PGOOD_B;
    }

    if(role.compare("UD_B") == 0)
    {
        gpio.role = CFG_ROLE_UD_B;
    }

    if(role.compare("REQ_OFF_B") == 0)
    {
        gpio.role = CFG_ROLE_REQ_OFF_B;
    }

    if(role.compare("D1_B") == 0)
    {
        gpio.role = CFG_ROLE_D1_B;
    }

    if(role.compare("D2_B") == 0)
    {
        gpio.role = CFG_ROLE_D2_B;
    }

    if(node.FindValue("direction"))
    {
        string direction;
        node["direction"] >> direction;

        validate_gpio_direction(direction);

        if(direction.compare("input") == 0)
        {
            gpio.direction = INPUT;
        }

        if(direction.compare("output") == 0)
        {
            gpio.direction = OUTPUT;
        }
    }
    else
    {
        gpio.direction = CFG_NOVAL;
    }

    if(node.FindValue("resistor"))
    {
        string resistor;
        node["resistor"] >> resistor;

        validate_gpio_resistor(resistor);

        if(resistor.compare("pull_up") == 0)
        {
            gpio.resistor = PUD_UP;
        }

        if(resistor.compare("pull_down") == 0)
        {
            gpio.resistor = PUD_DOWN;
        }

        if(resistor.compare("none") == 0)
        {
            gpio.resistor = PUD_OFF;
        }
    }
    else
    {
            gpio.resistor = CFG_NOVAL;
    }
}

void operator>>(const Node& node, config_socket &socket)
{
    node["port"] >> socket.port;
    validate_port(socket.port);
}

void operator>>(const Node& node, config_websocket &websocket)
{
    node["port"] >> websocket.port;
    validate_port(websocket.port);

    node["resource-path"] >> websocket.resource_path;
    validate_directory(websocket.resource_path);
}

void operator>>(const Node& node, vector<int>  &pm)
{
    for(unsigned int i=0; i < node.size(); i++)
    {
        int element = -1;
        node[i] >> element;
        pm.push_back(element);
    }
}

#include <syslog.h>

void operator>>(const Node& node, config_battery_level_reader &battery_level_reader)
{
    node["battery-level-read-interval"] >> battery_level_reader.battery_level_read_interval;
    validate_delay(battery_level_reader.battery_level_read_interval);

    node["battery-level-gpio-delay"] >> battery_level_reader.battery_level_gpio_delay;
    validate_delay(battery_level_reader.battery_level_gpio_delay);

    node["battery-level-pgood-delay"] >> battery_level_reader.battery_level_pgood_delay;
    validate_delay(battery_level_reader.battery_level_pgood_delay);

    string battery_level_log = "";
    node["battery-level-log"] >> battery_level_log;

    if(battery_level_log.compare("true") == 0)
    {
        battery_level_reader.battery_level_log = true;
    }
    else
    {
        battery_level_reader.battery_level_log = false;
    }
}

//------------------------------------------------------------------------------
// Input validation
//------------------------------------------------------------------------------

void validate_gpio_pin(int pin_no)
{
    vector<int> legal_bcm_pi_pins 
        {2,  3,  4,  17, 27, 22, 10, 9, 11,  5,  6, 13, 19, 26,
         14, 15, 18, 23, 24, 25,  8, 7, 12, 16, 20, 21};

    for(unsigned int i = 0; i < legal_bcm_pi_pins.size(); i++)
    {
        if(pin_no == legal_bcm_pi_pins[i])
        {
            return;
        }
    }

    throw runtime_error(error_gpio_msg_pin_no(pin_no));
}

void validate_gpio_direction(string direction)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The value of the direction field can only "
           "be [input|output].";

    if(direction.compare("input") != 0 && direction.compare("output") != 0)
    {
        throw runtime_error(msg.str());
    }
}

void validate_gpio_resistor(string resistor)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The value of the resistor field can only "
           "be [pull_up|pull_down|none].";

    if(resistor.compare("pull_up") != 0 && 
       resistor.compare("pull_down") != 0 && 
       resistor.compare("none") != 0)
    {
        throw runtime_error(msg.str());
    }
}

void validate_gpio_role(string role)
{
    int pin_count = 7;

    string legal_roles[] = 
        {"CS_B",
         "OFF",
         "PGOOD_B",
         "UD_B",
         "REQ_OFF_B",
         "D1_B",
         "D2_B",};

    for(int i = 0; i < pin_count; i++)
    {
        if(legal_roles[i].compare(role) == 0)
        {
            return;
        }
    }

    throw runtime_error(error_gpio_msg_role(role));
}

void validate_gpio_duplicate_pins(vector<config_gpio> gpios)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The GPIO pin #";

    for(unsigned int i=0; i<gpios.size(); i++)
    {
        for(unsigned int j=0; j<gpios.size(); j++)
        {
            if(gpios[i].bcm_pin == gpios[j].bcm_pin && i != j)
            {
                msg << gpios[i].bcm_pin << " has already been configured.";
                throw runtime_error(msg.str());
            } 
        }
    }
}


void validate_gpio_roles_vs_direction(vector<config_gpio> gpios)
{
    map<int, int> role;

    role[CFG_ROLE_CS_B]      = OUTPUT;
    role[CFG_ROLE_OFF]       = OUTPUT;
    role[CFG_ROLE_PGOOD_B]   = INPUT;
    role[CFG_ROLE_UD_B]      = INPUT;
    role[CFG_ROLE_REQ_OFF_B] = INPUT;
    role[CFG_ROLE_D1_B]      = INPUT;
    role[CFG_ROLE_D2_B]      = INPUT;

    map<int, string> msg;
    msg[CFG_ROLE_CS_B]       = "The CS_B* pin should be an output.";
    msg[CFG_ROLE_OFF]        = "The OFF pin should be an output.";
    msg[CFG_ROLE_PGOOD_B]    = "The PGOOD_B pin should be an input.";
    msg[CFG_ROLE_UD_B]       = "The UD_B* pin should be an input.";
    msg[CFG_ROLE_REQ_OFF_B]  = "The REQ_OFF_B pin should be an input";
    msg[CFG_ROLE_D1_B]       = "The D1_B pin should be an input.";
    msg[CFG_ROLE_D2_B]       = "The D2_B pin should be an input.";

    for(unsigned int i=0; i<gpios.size(); i++)
    {
        if(role[gpios[i].role] != gpios[i].direction)
        {
            throw runtime_error(msg[gpios[i].role]);
        }
    }
}

void validate_port(int port)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The range of legal port numbers is between 0 and 65535.";

    if( !(0x0 <= port && port <= 0xFFFF))
    {
        throw runtime_error(msg.str());
    }
}

void validate_directory(string path)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The directory: " << path;

    if(access(path.c_str(), F_OK) < 0)
    {
        msg << " does not exist.";
        throw runtime_error(msg.str());
    }

    if(access(path.c_str(), R_OK) < 0)
    {
        msg << " is not readable.";
        throw runtime_error(msg.str());
    }
}

void validate_delay(int delay)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "A time delay value must be greater than 0." << endl;

    if(delay <= 0)
    {
        throw runtime_error(msg.str());
    }
}

//------------------------------------------------------------------------------
// Error messages.
//------------------------------------------------------------------------------

string error_gpio_msg_sequence()
{
    stringstream retval;

    retval << CONFIG_ERR_PREFIX << "The GPIO field must be followed by a "
              "sequnece of GPIO no more that 40 profile objects:\n\n"

              "GPIO:\n"
              "  - bcm-pin: 1\n"
              "    direction: input\n"
              "    resistor: up\n"
              "  - bcm-pin: 2\n"
              "    direction: output\n"
              "    resistor: down\n"
              "    ...etc.";

    return retval.str();
}

string error_gpio_msg_pin_no(int pin_no)
{
    stringstream retval;

    retval << CONFIG_ERR_PREFIX << "Invalid GPIO pin number: '" << pin_no << 
              "', Legal Broadcom GPIO pin numbers are:\n"
              "    2,  3,  4,  17, 27, 22, 10, 9, 11,  5,  6, 13, 19, 26,\n"
              "    14, 15, 18, 23, 24, 25,  8, 7, 12, 16, 20, 21";

    return retval.str();
}

string error_gpio_msg_role(string role)
{
    stringstream retval;

    retval << CONFIG_ERR_PREFIX << "Invalid GPIO role: '" << role << "', Legal roles for GPIO pins are:\n"
              "    CS_B, OFF, PGOOD_B, UD_B, REQ_OFF_B, D1_B and D2_B";

    return retval.str();
}
