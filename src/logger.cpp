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

   File name: logger.cpp
   Date:      2015-08-26 20:43 
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <iostream>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "logger.h"

using namespace std;

logger::logger(string log_file_path, string log_file_name)
{
    this->log_file_path = log_file_path;

    char last_char = this->log_file_path.back();
    if(last_char != '/')
    {
        this->log_file_path.append("/");
    }

    if(access(this->log_file_path.c_str(), W_OK) < 0)
    {
        string msg = "Unable to write to log directory '";
        msg.append(log_file_path);
        msg.append("'. ");
        msg.append(strerror(errno));
        msg.append(".");
        throw runtime_error(msg.c_str());
    }

    this->log_file_path.append(log_file_name);

    string new_name_with_path = this->log_file_path;
    new_name_with_path.append(".0");

    struct stat sb;

    // If and only if an old rotated log file exists, and it is a regular then file delete it.
    if(access(new_name_with_path.c_str(), F_OK) == 0 &&
       stat(new_name_with_path.c_str(), &sb) == 0 &&
       S_ISREG(sb.st_mode))
    {
        remove(new_name_with_path.c_str());
    }

    // If the log file from the last time the process ran exists rotate it by renaming it.
    if(access(this->log_file_path.c_str(), F_OK) == 0)
    {
        rename(this->log_file_path.c_str(), new_name_with_path.c_str());
    }

    // Finally open a log file to be used the current incarnation of the parent process.
    log_file.open(this->log_file_path);

    if(log_file.fail())
    {
        string msg = "Unable open log file '";
        msg.append(log_file_path);
        msg.append("'. ");
        msg.append(strerror(errno));
        msg.append(".");
        throw runtime_error(msg.c_str());
    }
}

logger::~logger()
{
    log_file.close();
}

void logger::log(string msg)
{
    time_t now;
    time(&now);
    char iso8601_date[20];
    strftime(iso8601_date, sizeof(iso8601_date), "%Y-%m-%d %H:%M:%S", gmtime(&now));

    string line = iso8601_date;
    line.append(": ");
    line.append(msg);

    log_file << line << endl;

    if(log_file.fail())
    {
        string msg = "Unable write to file '";
        msg.append(log_file_path);
        msg.append("'. ");
        msg.append(strerror(errno));
        msg.append(".");
        throw runtime_error(msg.c_str());
    }
}
