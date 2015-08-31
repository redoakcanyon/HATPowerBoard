#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unistd.h>
#include <map>
#include <wiringPi.h>

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
void operator>>(const Node& node, config_power_level_reader &power_level_reader);

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

                        case CFG_ROLE_REQ_OFF:
                            req_off = gpio;
                            break;

                        case CFG_ROLE_CS:
                            cs = gpio;
                            break;

                        case CFG_ROLE_UD:
                            ud = gpio;
                            break;

                        case CFG_ROLE_PGOOD:
                            pgood = gpio;
                            break;

                        case CFG_ROLE_D1:
                            d1 = gpio;
                            break;

                        case CFG_ROLE_D2:
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

            if(key.compare("power-level-reader") == 0)
            {
                doc_value[0] >> power_level_reader;
            }
        }
    }

    //validate_gpio_duplicate_pins(gpios);
    //validate_gpio_roles_vs_direction(gpios);

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

        case CFG_ROLE_REQ_OFF:
            retval = req_off;
            break;

        case CFG_ROLE_CS:
            retval = cs;
            break;

        case CFG_ROLE_UD:
            retval = ud;
            break;

        case CFG_ROLE_PGOOD:
            retval = pgood;
            break;

        case CFG_ROLE_D1:
            retval = d1;
            break;

        case CFG_ROLE_D2:
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
    node["wipi-pin"] >> gpio.wipi_pin;
    validate_gpio_pin(gpio.wipi_pin);

    string role;
    node["role"] >> role;
    validate_gpio_role(role);

    if(role.compare("CS") == 0)
    {
        gpio.role = CFG_ROLE_CS;
    }

    if(role.compare("OFF") == 0)
    {
        gpio.role = CFG_ROLE_OFF;
    }

    if(role.compare("PGOOD") == 0)
    {
        gpio.role = CFG_ROLE_PGOOD;
    }

    if(role.compare("UD") == 0)
    {
        gpio.role = CFG_ROLE_UD;
    }

    if(role.compare("REQ_OFF") == 0)
    {
        gpio.role = CFG_ROLE_REQ_OFF;
    }

    if(role.compare("D1") == 0)
    {
        gpio.role = CFG_ROLE_D1;
    }

    if(role.compare("D2") == 0)
    {
        gpio.role = CFG_ROLE_D2;
    }

    string direction;
    node["direction"] >> direction;

    validate_gpio_direction(direction);

    if(direction.compare("input") == 0)
    {
        //gpio.direction = CFG_DIR_INPUT;
        gpio.direction = INPUT;
    }

    if(direction.compare("output") == 0)
    {
        //gpio.direction = CFG_DIR_OUTPUT;
        gpio.direction = OUTPUT;
    }


    string resistor;
    node["resistor"] >> resistor;

    validate_gpio_resistor(resistor);

    if(resistor.compare("pull_up") == 0)
    {
        //gpio.resistor = CFG_RES_PULL_UP;
        gpio.resistor = PUD_UP;
    }

    if(resistor.compare("pull_down") == 0)
    {
        //gpio.resistor = CFG_RES_PULL_DOWN;
        gpio.resistor = PUD_DOWN;
    }

    if(resistor.compare("none") == 0)
    {
        //gpio.resistor = CFG_RES_NONE;
        gpio.resistor = PUD_OFF;
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

void operator>>(const Node& node, config_power_level_reader &power_level_reader)
{
    node["power-level-read-interval"] >> power_level_reader.power_level_read_interval;
    validate_delay(power_level_reader.power_level_read_interval);

    node["power-level-gpio-delay"] >> power_level_reader.power_level_gpio_delay;
    validate_delay(power_level_reader.power_level_gpio_delay);

    string power_level_log = "";
    node["power-level-log"] >> power_level_log;

    if(power_level_log.compare("true") == 0)
    {
        power_level_reader.power_level_log = true;
    }
    else // Any value other than true is false.
    {
        power_level_reader.power_level_log = false;
    }
}

//------------------------------------------------------------------------------
// Input validation
//------------------------------------------------------------------------------

void validate_gpio_pin(int pin_no)
{
    int pin_count = 28;

    int legal_wiring_pi_pins[] = 
        {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16,
             21, 22, 23, 24, 25, 26, 27, 28, 29, 
         30, 31};

    for(int i = 0; i < pin_count; i++)
    {
        if(pin_no == legal_wiring_pi_pins[i])
        {
            return;
        }
    }

    throw runtime_error(error_gpio_msg_pin_no(pin_no));
}

void validate_gpio_direction(string direction)
{
    stringstream msg;

    msg << CONFIG_ERR_PREFIX << "The value of the direction field can only"
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
        {"CS",
         "OFF",
         "PGOOD",
         "UD",
         "REQ_OFF",
         "D1",
         "D2",};

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

    msg << CONFIG_ERR_PREFIX << "The WiringPI pin #";

    for(unsigned int i=0; i<gpios.size(); i++)
    {
        for(unsigned int j=0; j<gpios.size(); j++)
        {
            if(gpios[i].wipi_pin == gpios[j].wipi_pin && i != j)
            {
                msg << gpios[i].wipi_pin << " has already been configured.";
                throw runtime_error(msg.str());
            } 
        }
    }
}


void validate_gpio_roles_vs_direction(vector<config_gpio> gpios)
{
    map<int, int> role;

    role[CFG_ROLE_CS]      = OUTPUT;
    role[CFG_ROLE_OFF]     = OUTPUT;
    role[CFG_ROLE_PGOOD]   = INPUT;
    role[CFG_ROLE_UD]      = INPUT;
    role[CFG_ROLE_REQ_OFF] = INPUT;
    role[CFG_ROLE_D1]      = INPUT;
    role[CFG_ROLE_D2]      = INPUT;

    map<int, string> msg;
    msg[CFG_ROLE_CS]      = "The CS* pin should be an output.";
    msg[CFG_ROLE_OFF]     = "The OFF pin should be an output.";
    msg[CFG_ROLE_PGOOD]   = "The PGOOD pin should be an input.";
    msg[CFG_ROLE_UD]      = "The UD* pin should be an input.";
    msg[CFG_ROLE_REQ_OFF] = "The REQ_OFF pin should be an input";
    msg[CFG_ROLE_D1]      = "The D1 pin should be an input.";
    msg[CFG_ROLE_D2]      = "The D2 pint should be an input.";

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
              "  - wipi-in: 1\n"
              "    direction: input\n"
              "    resistor: up\n"
              "  - wipi-pin: 2\n"
              "    direction: output\n"
              "    resistor: down\n"
              "    ...etc.";

    return retval.str();
}

string error_gpio_msg_pin_no(int pin_no)
{
    stringstream retval;

    retval << CONFIG_ERR_PREFIX << "Invalid WiringPI pin number: '" << pin_no << 
    "', Legal WiringPI GPIO pin numbers are:\n"
              "    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,\n"
              "    10, 11, 12, 13, 14, 15, 16,\n"
              "        21, 22, 23, 24, 25, 26, 27, 28, 29, \n"
              "    30, 31";

    return retval.str();
}

string error_gpio_msg_role(string role)
{
    stringstream retval;

    retval << CONFIG_ERR_PREFIX << "Invalid GPIO role: '" << role << "', Legal roles for WiringPI GPIO pins are:\n"
              "    CS, OFF, PGOOD, UD, REQ_OFF, D1 and D2";

    return retval.str();
}
