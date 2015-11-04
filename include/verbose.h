/*
   -----------------------------------------------------------------------------

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
   Date:      2015-08-04 10:33
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#ifndef __VERBOSE_H__
#define __VERBOSE_H__


#include "command-line.h"
#include "config.h"

void verbose_print_config(config *conf);
void verbose_print_options(cmdline_options opts);

#endif
