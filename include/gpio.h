#ifndef __GPIO_H__
#define __GPIO_H__

#include "config.h"

void gpio_init(config *conf);
void gpio_write_off(config *conf);
int gpio_read_req_off(config *conf);
int gpio_read_d1(config *conf);
int gpio_read_d2(config *conf);
int gpio_read_battery_level_raw(config *conf);

#endif
