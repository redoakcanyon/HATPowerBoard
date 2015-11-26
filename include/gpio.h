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

   File name: gpio.h
   Date:      2015-08-11 13:29
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "config.h"

#define GPIO_ERROR   -2
#define CHARGING     -1
#define GPIO_SUCCESS  7

#define LOW      0
#define HIGH     1
#define PUD_UP   2
#define PUD_DOWN 3
#define PUD_OFF  4
#define INPUT    5
#define OUTPUT   6

int gpio_init(config *conf);
int gpio_write_off(config *conf);
int gpio_read_req_off(config *conf);
int gpio_read_d1(config *conf);
int gpio_read_d2(config *conf);
int gpio_read_battery_level_raw(config *conf);

#endif
