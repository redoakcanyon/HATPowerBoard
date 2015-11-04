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

   File name: command-line.h
   Date:      2015-08-07 21:59
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#include <string>

class cmdline_options
{
    private:
        int power_off_flag;
        int websocket_flag;
        int berkleysocket_flag;
        int battery_level_flag;
        int battery_level_raw_flag;
        int verbose_flag;
        int help_flag;
        std::string config_path;

    public:
        cmdline_options(int argc, char **argv);

        bool is_power_off();
        bool is_websocket();
        bool is_berkleysocket();
        bool is_battery_level();
        bool is_battery_level_raw();
        bool is_verbose();
        bool is_help();
        std::string get_config_path();
        static void usage();
};

#endif
