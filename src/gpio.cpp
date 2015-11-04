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
   Date:      2015-08-11 13:26
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <wiringPi.h>
#include "gpio.h"

#define DELAY(delay) usleep(delay)

using namespace std;

void gpio_init(config *conf)
{
    // OFF -> 26
    // output, none
    config_gpio off = conf->get_gpio_by_role(CFG_ROLE_OFF);
    pinMode (off.wipi_pin, off.direction);

    // REQ_OFF -> 21
    // input, pull_up
    config_gpio req_off = conf->get_gpio_by_role(CFG_ROLE_REQ_OFF);
    pinMode (req_off.wipi_pin, req_off.direction);
    pullUpDnControl(req_off.wipi_pin, req_off.resistor);

    // CS -> 22
    // output, none
    config_gpio cs = conf->get_gpio_by_role(CFG_ROLE_CS);
    pinMode (cs.wipi_pin, cs.direction);
    digitalWrite(cs.wipi_pin, HIGH);

    // UD -> 27
    // output, none
    config_gpio ud = conf->get_gpio_by_role(CFG_ROLE_UD);
    pinMode (ud.wipi_pin, ud.direction);
    //digitalWrite(ud.wipi_pin, HIGH);

    // PGOOD -> 23
    // input, pull_up
    config_gpio pgood = conf->get_gpio_by_role(CFG_ROLE_PGOOD);
    pinMode (pgood.wipi_pin, pgood.direction);
    pullUpDnControl(pgood.wipi_pin, pgood.resistor);

    // D1 -> 28
    config_gpio d1 = conf->get_gpio_by_role(CFG_ROLE_D1);
    pinMode (d1.wipi_pin, d1.direction);
    pullUpDnControl(d1.wipi_pin, d1.resistor);

    // D2 -> 25
    config_gpio d2 = conf->get_gpio_by_role(CFG_ROLE_D2);
    pinMode (d2.wipi_pin, d2.direction);
    pullUpDnControl(d2.wipi_pin, d2.resistor);
}

void gpio_write_off(config *conf)
{
    int pin_no_off = conf->get_gpio_by_role(CFG_ROLE_OFF).wipi_pin;

    for (int i=0; i<3;i++)
    {
        // Delay must be longer here because of debounce.
        digitalWrite (pin_no_off, HIGH) ;
        usleep(200000);
        digitalWrite (pin_no_off,  LOW) ;
        usleep(200000);
    }
}

// Returns 0 if the OFF button has been pressed by the user, until then it returns 1.
int gpio_read_req_off(config *conf)
{
    int pin_no_req_off = conf->get_gpio_by_role(CFG_ROLE_REQ_OFF).wipi_pin;

    return digitalRead (pin_no_req_off);
}

// Returns 1 if power board is charging 0 if it is not.
int gpio_read_d1(config *conf)
{
    int pin_no_d1 = conf->get_gpio_by_role(CFG_ROLE_D1).wipi_pin;

    return !digitalRead (pin_no_d1);
}

// Returns 1 if the wall wart is connected 0 if it is not.
int gpio_read_d2(config *conf)
{
    int pin_no_d2 = conf->get_gpio_by_role(CFG_ROLE_D2).wipi_pin;

    return !digitalRead (pin_no_d2);
}

int gpio_read_battery_level_raw(config *conf)
{
    int pin_no_cs = conf->get_gpio_by_role(CFG_ROLE_CS).wipi_pin;
    int pin_no_ud = conf->get_gpio_by_role(CFG_ROLE_UD).wipi_pin;
    int pin_no_pgood = conf->get_gpio_by_role(CFG_ROLE_PGOOD).wipi_pin;
    int delay = conf->get_battery_level_reader().battery_level_gpio_delay;

    int status_pgood = digitalRead(pin_no_pgood);
    DELAY(delay);

    if(gpio_read_d2(conf))
    {
        return -1;
    }
    DELAY(delay);

    digitalWrite(pin_no_ud, HIGH);
    DELAY(delay);

    digitalWrite(pin_no_cs, LOW);
    DELAY(delay);

    for(int i=0; i<=63; i++)
    {
        digitalWrite(pin_no_ud, LOW);
        DELAY(delay);

        digitalWrite(pin_no_ud, HIGH);
        DELAY(delay);

        status_pgood = digitalRead(pin_no_pgood);
    }

    digitalWrite(pin_no_cs, HIGH);
    DELAY(delay);

    status_pgood = digitalRead(pin_no_pgood);
    DELAY(delay);

    digitalWrite(pin_no_ud, LOW);
    DELAY(delay);

    digitalWrite(pin_no_cs, LOW);
    DELAY(delay);

    status_pgood = digitalRead(pin_no_pgood);

    int battlevel = 63;

    while(status_pgood == 1 && battlevel > 0)
    {
        digitalWrite(pin_no_ud, HIGH);
        DELAY(delay);

        digitalWrite(pin_no_ud, LOW);
        DELAY(delay);

        status_pgood = digitalRead(pin_no_pgood);

        battlevel--;
    }

    digitalWrite(pin_no_cs, HIGH);
    DELAY(delay);

    return battlevel;
}

