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

   File name: logger.h
   Date:      2015-08-26 20:48
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <fstream>

using namespace std;

class logger
{
    private:
        string log_file_path;
        ofstream log_file;

    public:
        logger(string log_file_path, string log_file_name);
        ~logger();
        void log(string msg);
};

#endif
