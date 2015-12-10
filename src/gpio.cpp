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

File name: gpio.cpp
Date:      2015-11-16 19:26
Author:    Kristjan Runarsson

-----------------------------------------------------------------------------
*/

#include <unistd.h>
#include <syslog.h>

#include <fstream>

#include "gpio.h"

#define DELAY(delay) usleep(delay)
#define PGOOD_SETTLE_DELAY 500000

using namespace std;

string gpio_map_direction(int direction)
{
    switch(direction)
    {
        case INPUT:
            return "in";
        case OUTPUT:
            return "out";
        default:
            return "off";
    }
}

int digitalWrite(int gpionum, int val)
{
    string value = to_string(val);
    string gpionumber = to_string(gpionum);

    string gpio_val_str = "/sys/class/gpio/gpio" + gpionumber + "/value";
    ofstream gpio(gpio_val_str);

    if (gpio < 0)
    {
        return GPIO_ERROR;
    }

    gpio << value.c_str();
    gpio.close();

    return GPIO_SUCCESS;
}

int digitalRead(int gpionum)
{
    string gpionumber = to_string(gpionum);

    string gpio_val_str = "/sys/class/gpio/gpio" + gpionumber + "/value";
    ifstream gpio(gpio_val_str);

    if (gpio < 0)
    {
        return GPIO_ERROR;
    }

    string val;
    gpio >> val ;  //read gpio value

    gpio.close(); //close the value file

    if(val == "1")
    {
        return HIGH ;
    }

    return LOW;
}

int gpio_init_pin(int pin_no)
{
    ofstream export_sysfs("/sys/class/gpio/export");

    if (export_sysfs < 0)
    {
        return GPIO_ERROR;
    }

    export_sysfs << pin_no;
    export_sysfs.close();

    return GPIO_SUCCESS;
}

int gpio_cleanup_pin(int pin_no)
{
    ofstream unexport_sysfs("/sys/class/gpio/unexport");

    if (unexport_sysfs < 0)
    {
        return GPIO_ERROR;
    }

    unexport_sysfs << pin_no;
    unexport_sysfs.close();

    return GPIO_SUCCESS;
}

int gpio_set_pin_direction(int pin_no, int dir)
{
    string pin_number = to_string(pin_no);
    string direction_path = "/sys/class/gpio/gpio" + pin_number + "/direction";
    ofstream direction_sysfs(direction_path);

    if (direction_sysfs < 0)
    {
        return GPIO_ERROR;
    }

    direction_sysfs << gpio_map_direction(dir) << endl;;
    direction_sysfs.close();

    return GPIO_SUCCESS;
}

int gpio_init(config *conf)
{
    int status = GPIO_SUCCESS;

    // OFF
    // output, none
    config_gpio off = conf->get_gpio_by_role(CFG_ROLE_OFF);

    syslog(LOG_INFO, "Initializing OFF");

    status = gpio_init_pin(off.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(off.bcm_pin, OUTPUT);
    if(status == GPIO_ERROR) { return status; }

    // REQ_OFF
    // input, pull_up
    config_gpio req_off = conf->get_gpio_by_role(CFG_ROLE_REQ_OFF_B);

    syslog(LOG_INFO, "Initializing REQ_OFF");

    status = gpio_init_pin(req_off.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(req_off.bcm_pin, INPUT);
    if(status == GPIO_ERROR) { return status; }

    // CS
    // output, none
    config_gpio cs = conf->get_gpio_by_role(CFG_ROLE_CS_B);

    syslog(LOG_INFO, "Initializing CS");

    status = gpio_init_pin(cs.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(cs.bcm_pin, OUTPUT);
    if(status == GPIO_ERROR) { return status; }
    status = digitalWrite(cs.bcm_pin, HIGH);
    if(status == GPIO_ERROR) { return status; }

    // UD
    // output, none
    config_gpio ud = conf->get_gpio_by_role(CFG_ROLE_UD_B);

    syslog(LOG_INFO, "Initializing UD");

    status = gpio_init_pin(ud.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(ud.bcm_pin, OUTPUT);
    if(status == GPIO_ERROR) { return status; }

    // PGOOD
    // input, pull_up
    config_gpio pgood = conf->get_gpio_by_role(CFG_ROLE_PGOOD_B);

    syslog(LOG_INFO, "Initializing PGOOD");

    status = gpio_init_pin(pgood.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(pgood.bcm_pin, INPUT);
    if(status == GPIO_ERROR) { return status; }

    // D1 (Charge indicator)
    // input, pull_up
    config_gpio d1 = conf->get_gpio_by_role(CFG_ROLE_D1_B);

    syslog(LOG_INFO, "Initializing D1");

    status = gpio_init_pin(d1.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(d1.bcm_pin, INPUT);
    if(status == GPIO_ERROR) { return status; }

    // D2 (Power connect indicator)
    // input, pull_up
    config_gpio d2 = conf->get_gpio_by_role(CFG_ROLE_D2_B);

    syslog(LOG_INFO, "Initializing D2");

    status = gpio_init_pin(d2.bcm_pin);
    if(status == GPIO_ERROR) { return status; }
    status = gpio_set_pin_direction(d2.bcm_pin, INPUT);
    if(status == GPIO_ERROR) { return status; }

    syslog(LOG_INFO, "Done initializing GPIO");

    return GPIO_SUCCESS;
}

int gpio_cleanup(config *conf)
{
    int status = GPIO_SUCCESS;

    // OFF
    // output, none
    config_gpio off = conf->get_gpio_by_role(CFG_ROLE_OFF);

    syslog(LOG_INFO, "Cleaning up OFF");

    status = gpio_cleanup_pin(off.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // REQ_OFF
    // input, pull_up
    config_gpio req_off = conf->get_gpio_by_role(CFG_ROLE_REQ_OFF_B);

    syslog(LOG_INFO, "Cleaning up REQ_OFF");

    status = gpio_cleanup_pin(req_off.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // CS
    // output, none
    config_gpio cs = conf->get_gpio_by_role(CFG_ROLE_CS_B);

    syslog(LOG_INFO, "Cleaning up CS");

    status = gpio_cleanup_pin(cs.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // UD
    // output, none
    config_gpio ud = conf->get_gpio_by_role(CFG_ROLE_UD_B);

    syslog(LOG_INFO, "Cleaning up UD");

    status = gpio_cleanup_pin(ud.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // PGOOD
    // input, pull_up
    config_gpio pgood = conf->get_gpio_by_role(CFG_ROLE_PGOOD_B);

    syslog(LOG_INFO, "Cleaning up PGOOD");

    status = gpio_cleanup_pin(pgood.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // D1 (Charge indicator)
    // input, pull_up
    config_gpio d1 = conf->get_gpio_by_role(CFG_ROLE_D1_B);

    syslog(LOG_INFO, "Cleaning up D1");

    status = gpio_cleanup_pin(d1.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    // D2 (Power connect indicator)
    // input, pull_up
    config_gpio d2 = conf->get_gpio_by_role(CFG_ROLE_D2_B);

    syslog(LOG_INFO, "Cleaning up D2");

    status = gpio_cleanup_pin(d2.bcm_pin);
    if(status == GPIO_ERROR) { return status; }

    syslog(LOG_INFO, "Done cleaning up GPIO");

    return GPIO_SUCCESS;
}

int gpio_write_off(config *conf)
{
    int status = GPIO_SUCCESS;

    syslog(LOG_CRIT, "calling gpio_write_off");
    int pin_no_off = conf->get_gpio_by_role(CFG_ROLE_OFF).bcm_pin;

    for (int i=0; i<3;i++)
    {
        // Delay must be longer here because of debounce.
        status = digitalWrite (pin_no_off, HIGH) ;
        if(status == GPIO_ERROR) { return status; }
        usleep(200000);
        status = digitalWrite (pin_no_off,  LOW) ;
        if(status == GPIO_ERROR) { return status; }
        usleep(200000);
    }

    return GPIO_SUCCESS;
}

// Returns 0 if the OFF button has been pressed by the user, until then it returns 1.
int gpio_read_req_off(config *conf)
{
    int pin_no_req_off = conf->get_gpio_by_role(CFG_ROLE_REQ_OFF_B).bcm_pin;

    int status = GPIO_SUCCESS;
    status = digitalRead (pin_no_req_off);
    if(status == GPIO_ERROR) { return status; }

    return !status;
}

// Returns 1 if power board is charging 0 if it is not.
int gpio_read_d1(config *conf)
{
    int pin_no_d1 = conf->get_gpio_by_role(CFG_ROLE_D1_B).bcm_pin;

    int status = GPIO_SUCCESS;
    status = digitalRead (pin_no_d1);
    if(status == GPIO_ERROR) { return status; }

    return !status;
}

// Returns 1 if the wall wart is connected 0 if it is not.
int gpio_read_d2(config *conf)
{
    int pin_no_d2 = conf->get_gpio_by_role(CFG_ROLE_D2_B).bcm_pin;

    int status = GPIO_SUCCESS;
    status = digitalRead (pin_no_d2); 
    if(status == GPIO_ERROR) { return status; }

    return !status;
}

int gpio_read_battery_level_raw(config *conf)
{
    int pin_no_cs = conf->get_gpio_by_role(CFG_ROLE_CS_B).bcm_pin;
    int pin_no_ud = conf->get_gpio_by_role(CFG_ROLE_UD_B).bcm_pin;
    int pin_no_pgood = conf->get_gpio_by_role(CFG_ROLE_PGOOD_B).bcm_pin;
    int delay = conf->get_battery_level_reader().battery_level_gpio_delay;

    int status_pgood = digitalRead(pin_no_pgood);
    if(status_pgood == GPIO_ERROR) { return status_pgood; }
    DELAY(delay);

    int status = GPIO_SUCCESS;

    if(gpio_read_d2(conf))
    {
        return CHARGING; 
    }
    DELAY(delay);
    
    // reset the comparator

    status = digitalWrite(pin_no_ud, HIGH);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);

    status = digitalWrite(pin_no_cs, LOW);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);

    for(int i=0; i<=63; i++)
    {
        status = digitalWrite(pin_no_ud, LOW);
        if(status == GPIO_ERROR) { return status; }
        DELAY(delay);

        status = digitalWrite(pin_no_ud, HIGH);
        if(status == GPIO_ERROR) { return status; }
        DELAY(delay);

        status_pgood = digitalRead(pin_no_pgood);
        if(status_pgood == GPIO_ERROR) { return status_pgood; }
    }

    status = digitalWrite(pin_no_cs, HIGH);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);
    DELAY(PGOOD_SETTLE_DELAY); // force a  1/2 second delay for pgood to settle in case no battery is attached.

    status_pgood = digitalRead(pin_no_pgood);
    if(status_pgood == GPIO_ERROR) { return status_pgood; }
    DELAY(delay);

    status = digitalWrite(pin_no_ud, LOW);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);

    status = digitalWrite(pin_no_cs, LOW);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);
    
    // read the level now

    status_pgood = digitalRead(pin_no_pgood);
    if(status_pgood == GPIO_ERROR) { return status_pgood; }

    int battlevel = 63;

    while(status_pgood == 1 && battlevel > 0)
    {
        status = digitalWrite(pin_no_ud, HIGH);
        if(status == GPIO_ERROR) { return status; }
        DELAY(delay);

        status = digitalWrite(pin_no_ud, LOW);
        if(status == GPIO_ERROR) { return status; }
        DELAY(delay);

        status_pgood = digitalRead(pin_no_pgood);
        if(status == GPIO_ERROR) { return status; }
        if(status_pgood == GPIO_ERROR) { return status_pgood; }

        battlevel--;
    }

    status = digitalWrite(pin_no_cs, HIGH);
    if(status == GPIO_ERROR) { return status; }
    DELAY(delay);

    return battlevel;
}

