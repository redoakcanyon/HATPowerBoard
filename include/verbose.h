#ifndef __VERBOSE_H__
#define __VERBOSE_H__


#include "command-line.h"
#include "config.h"

void verbose_print_config(config *conf);
void verbose_print_options(cmdline_options opts);

#endif
