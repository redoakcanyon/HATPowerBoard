/*
 * File name: data-logger.cpp
 * Date:      2015-08-26 20:48
 * Author:    Kristján Rúnarsson
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
