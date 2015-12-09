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

   File name: rocpmd.h
   Date:      2015-12-09 21:12
   Author:    Kristján Rúnarsson

   -----------------------------------------------------------------------------
*/

#ifndef __ROCPMD_H__
#define __ROCPMD_H__

#include "tdaemon.h"
#include "ud_server.h"
#include "logger.h"

class rocpmd: public tdaemon
{

    public:
        rocpmd(string daemon_name, string lock_file_name, int daemon_flags);
        ~rocpmd();

    protected:
        bool power_log;
        ud_server *ud;
        logger *battery_level_log;
        struct timeval last_read;
        int daemon_main(config *conf);
        bool init_battery_level_timer();
        int battery_level_raw, battery_level_percent;
        void halt_system();
        bool battery_level_read_interval_expired(config *conf);
};

#endif
