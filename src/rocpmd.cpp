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

   File name: rocpmd.cpp
   Date:      2015-08-25 12:42
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h> // Strange things happen to POSIX threads if this is missing.
#include <errno.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <sys/time.h>

//#include "tdaemon.h"
#include "rocpmd.h"
#include "command-line.h"
#include "config.h"
#include "verbose.h"
#include "gpio.h"
//#include "ud_server.h"
#include "ud_client.h"
//#include "logger.h"

#define ROCPMD_LOCKFILE_MISSING       1
#define ROCPMD_LOCKFILE_NOT_READABLE  2
#define ROCPMD_PROCESS_LIVES          3
#define ROCPMD_PROCESS_DEAD           4


using namespace std;

// -----------------------------
// Globals
// -----------------------------

config *g_conf = NULL;
rocpmd *ROCPMD = NULL;

// -----------------------------
// Functions
// -----------------------------

void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGHUP || signo == SIGTERM)
    {
        if(g_conf)
        {
            gpio_cleanup(g_conf);
            delete(g_conf);
        }

        if(ROCPMD)
        {
            delete(ROCPMD);
        }

        exit(EXIT_SUCCESS);
    }
}

int rocpmd_instance_lives(string lockfile_path)
{
    // If the lock file does not exist we assume there is no living rocpmd instance.
    if(access(lockfile_path.c_str(), F_OK) < 0)
    {
        return ROCPMD_LOCKFILE_MISSING;
    }

    // If the lock file exists and is not readable we have a problem.
    if(access(lockfile_path.c_str(), R_OK) < 0)
    {
        return ROCPMD_LOCKFILE_NOT_READABLE;
    }

    // ... else we check if the pid in the lockfile is for a living process.

    string pid = "";

    ifstream lockfile(lockfile_path.c_str());

    getline(lockfile, pid);

    lockfile.close();

    int kill_stat = kill(atoi(pid.c_str()), 0);

    /* pid file exists and process lives */ 
    //if(kill(atoi(pid.c_str()), 0) == 0)
    if(kill_stat == 0)
    {
        return ROCPMD_PROCESS_LIVES;
    }

    return ROCPMD_PROCESS_DEAD;
}

void log_and_report(int priority, string prefix, string postfix)
{
	    stringstream msg;
        msg << prefix << postfix;
        cerr << msg.str() << endl;
		syslog(priority, msg.str().c_str());
}

int map_battery_level(config * conf, int battery_level_raw)
{
    int battery_level_percent = -1;

    // The battery level reading is not reliable when the wall wart is connected
    // in which case the gpio_read_battery_level_raw() function will return -1
    // so in the interest of uniformity the percentage power level is also set to
    // -1 to indicate charging status.
    if(battery_level_raw >= 0)
    {
        battery_level_percent = conf->get_powermap_element_at(battery_level_raw);
    }

    return battery_level_percent;
}

bool is_board_installed()
{
    string vendor_path = "/proc/device-tree/hat/vendor";
    string product_id_path = "/proc/device-tree/hat/product_id";

    string error = "";

    error = vendor_path;

    string no_board = " This could be due to a file system error but it most probably means "
                       "that no Red Oak Canyon Power Management Hat board is installed";

    if(access(vendor_path.c_str(), F_OK) == -1)
    {
        error.append(" does not exist.");
        error.append(no_board);
        log_and_report(LOG_CRIT, "Error in board detection: ", error.c_str());
        return false;
    }
    else if(access(vendor_path.c_str(), R_OK) == -1)
    {
        error.append(" is not readable.");
        error.append(no_board);
        log_and_report(LOG_CRIT, "Error in board detection: ", error.c_str());
        return false;
    }

    error = product_id_path;

    if(access(product_id_path.c_str(), F_OK) == -1)
    {
        error.append(" does not exist.");
        error.append(no_board);
        log_and_report(LOG_CRIT, "Error in board detection: ", error.c_str());
        return false;
    }
    else if(access(product_id_path.c_str(), R_OK) == -1)
    {
        error.append(" is not readable.");
        error.append(no_board);
        log_and_report(LOG_CRIT, "Error in board detection: ", error.c_str());
        error.append(no_board);
        return false;
    }

    string wrong_board = " You appear to have the wrong board installed.";

    ifstream vendor_file (vendor_path, fstream::in);
    string vendor = "";
    getline(vendor_file, vendor);
    vendor_file.close();
    vendor = vendor.c_str(); // Gets rid of the \0 character at the end.

    string vendor_expected = "Red Oak Canyon LLC";

    error = "the vendor name of the installed board '";
    error.append(vendor);
    error.append("' is incorrect.");
    error.append(wrong_board);

    int length = min(vendor.length(), vendor_expected.length());

    if(vendor.compare(0, length, vendor_expected) != 0)
    {
        log_and_report(LOG_CRIT, "Board validation failure: ", error);
        return false;
    }

    ifstream product_id_file (product_id_path, fstream::in);
    string product_id = "";
    getline(product_id_file, product_id);
    product_id_file.close();
    product_id = product_id.c_str(); // Gets rid of the \0 character at the end.

    string product_id_expected = "0x0001";

    error = "the product ID of the installed board '";
    error.append(product_id);
    error.append("' is incorrect.");
    error.append(wrong_board);

    length = min(product_id.length(), product_id_expected.length());

    if(product_id.compare(0, length, product_id_expected) != 0)
    {
        log_and_report(LOG_CRIT, "Board validation failure: ", error.c_str());
        return false;
    }

    return true;
}

// -----------------------------
// Classes and structures
// -----------------------------

/*
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
*/

rocpmd::rocpmd(string daemon_name, string lock_file_name, int daemon_flags)
: tdaemon(daemon_name, lock_file_name, daemon_flags)
{
    gettimeofday(&last_read , NULL);
}

rocpmd::~rocpmd()
{
    delete battery_level_log;
}

int rocpmd::daemon_main(config *conf)
{
    string wd = "";
	check_work_dir(wd);
	
    ud = new ud_server();

    bool create_power_log = conf->get_battery_level_reader().battery_level_log;

    if(create_power_log)
    {
        try
        {
            battery_level_log = new logger("/var/log", "rocpmd-battery-level.log");
        }
        catch(runtime_error re)
        {
            stringstream msg;
    	    msg << "Skipping power level log creation: " << re.what();
            log_and_report(LOG_CRIT, "Exiting: ", msg.str());
            create_power_log = false;
        }
    }

	stringstream msg;
	msg<<"Starting rocpmd (pid: " << get_pid() << ")";
    log_and_report(LOG_CRIT, msg.str(), "");

    battery_level_raw = gpio_read_battery_level_raw(conf);
    battery_level_percent = map_battery_level(conf, battery_level_raw);

    ud->set_battery_level_percent(battery_level_percent);
    ud->set_battery_level_raw(battery_level_raw);

	while(true)
    {
		usleep(100000);

        if(gpio_read_req_off(conf))
        {
            syslog(LOG_CRIT, "Halting due to gpio_read_req_off().");
            halt_system();
        }

        if(battery_level_read_interval_expired(conf))
        {
            battery_level_raw = gpio_read_battery_level_raw(conf);
            battery_level_percent = map_battery_level(conf, battery_level_raw);

            ud->set_battery_level_percent(battery_level_percent);
            ud->set_battery_level_raw(battery_level_raw);

            if(create_power_log)
            {
                try
                {
                    stringstream msg;
                    msg << battery_level_raw << "," << battery_level_percent << "%";
                    battery_level_log->log(msg.str());
                }
                catch(runtime_error re)
                {
                    log_and_report(LOG_CRIT, re.what(), "");
                }
            }
    
            if(battery_level_percent == 0)
            {
                syslog(LOG_CRIT, "Halting due to insufficient battery level.");
                halt_system();
            }
        }
    
        try
        {
            ud->handle_requests();
            if(ud->is_power_off())
            {
                gpio_write_off(conf);
            }
        }
        catch(runtime_error re)
        {
            log_and_report(LOG_CRIT, re.what(), "");
        }
	}

	return 0;
}

void rocpmd::halt_system()
{
    system("sudo /sbin/shutdown -h now");

    if(ROCPMD)
    {
        delete(ROCPMD);
    }

    exit(EXIT_SUCCESS);
}

#define SEC_TO_MSEC(x) x*1000.0

bool rocpmd::battery_level_read_interval_expired(config *conf)
{
    struct timeval now;

    double interval = SEC_TO_MSEC(conf->get_battery_level_reader().battery_level_read_interval);

    double diff, seconds, useconds;

    gettimeofday(&now , NULL);

    seconds  = now.tv_sec  - last_read.tv_sec;
    useconds = now.tv_usec - last_read.tv_usec;

    diff = ((seconds) * 1000 + useconds/1000.0);

    if(diff < interval)
    {
        return false;
    }

    last_read = now;

    return true;
}

// -----------------------------
// Main
// -----------------------------

int main(int argc, char **argv)
{
    bool opt_exit = false;

    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGTERM, sig_handler);

    string daemon_name = "rocpmd";
    string daemon_lockfile_name = daemon_name;
    daemon_lockfile_name.append(".pid");

    cmdline_options opts(argc, argv);

    if(!is_board_installed())
    {
        exit(EXIT_FAILURE);
    }

    if(opts.is_help())
    {
        cmdline_options::usage();
        exit(EXIT_SUCCESS);
    }

    try
    {
		ROCPMD = new rocpmd(daemon_name, daemon_lockfile_name, SINGLETON);
	}
	catch(tdaemon_exception e)
    {
        log_and_report(LOG_CRIT, "Exiting: ", e.what());
        exit(EXIT_FAILURE);
	}
	catch(...)
    {
		unexpected();
	}

    if(geteuid() != 0)
    {
	    stringstream msg;
        msg << "You must have root privileges to run the '" << daemon_name << "' daemon";
        log_and_report(LOG_CRIT, "Exiting: ", msg.str());
        exit (EXIT_FAILURE);
    }

    string config_path;

    if(opts.get_config_path().length())
    {
        config_path = opts.get_config_path();
    }
    
    try
    {        
        g_conf = new config(config_path);
    }
    catch(runtime_error e)
    {
        log_and_report(LOG_CRIT, "Exiting: ", e.what());
        return EXIT_FAILURE;
    }
    catch(...)
    {
        unexpected();
    }

    if(opts.is_verbose())
    {
        verbose_print_options(opts);
        verbose_print_config(g_conf);
    }
    int rocpmd_status = rocpmd_instance_lives(ROCPMD->get_lockfile_path());

    // if( rocpmd_status == ROCPMD_LOCKFILE_MISSING || 
    //     rocpmd_status != ROCPMD_PROCESS_LIVES )
    if(rocpmd_status != ROCPMD_PROCESS_LIVES )
    {
        log_and_report(LOG_CRIT, "Initializing GPIO...","" );
        gpio_init(g_conf);
    }

    if(opts.is_power_off() || 
       opts.is_battery_level() ||
       opts.is_battery_level_raw())
    {
        opt_exit = true;
    }

    if(opts.is_battery_level() || opts.is_battery_level_raw())
    {
        int raw_power = 0;

        rocpmd_status = rocpmd_instance_lives(ROCPMD->get_lockfile_path());

        if(rocpmd_status == ROCPMD_LOCKFILE_NOT_READABLE)
        {
	        stringstream msg;
            msg << "Unable to read the lockfile: " << ROCPMD->get_lockfile_path();
            log_and_report(LOG_CRIT, "Exiting: ", msg.str());
            exit(EXIT_FAILURE);
        }

        //if(rocpmd_status != ROCPMD_LOCKFILE_MISSING || rocpmd_status == ROCPMD_PROCESS_LIVES)
        if(rocpmd_status == ROCPMD_PROCESS_LIVES)
        {
            syslog(LOG_INFO, "Battery read by UD");

            if(opts.is_battery_level_raw())
            {
                int raw_power = ud_client_send_command(ROCPMD_SEND_POWER_LEVEL_RAW);

                if(raw_power >= 0)
                {
                    cout << "Raw power level:        " << raw_power << endl;
                }
                else
                {
                     cout << "Charging..." << endl;
                }
            }

            if(opts.is_battery_level())
            {
                int percent_power = ud_client_send_command(ROCPMD_SEND_POWER_LEVEL_PERCENT);
                
                if(percent_power >= 0)
                {
                    cout << "Percentage power level: " << percent_power << "%" << endl;
                }
                else
                {
                    cout << "Charging..." << endl ;
                }
            }
        }
        else
        {
            syslog(LOG_INFO, "Battery read by SYSFS");

            raw_power = gpio_read_battery_level_raw(g_conf);

            if(raw_power >= 0)
            {
                if(opts.is_battery_level_raw())
                {
                    cout << "Raw power level:        " << raw_power << endl;
                }

                if(opts.is_battery_level())
                {
                    int percent_power = g_conf->get_powermap_element_at(raw_power);
                    cout << "Percentage power level: " << percent_power << "%" << endl;
                }
            }
            else
            {
                cout << "Charging..." << endl;
            }
        }
    }

    if(opts.is_power_off())
    {
        log_and_report(LOG_CRIT, "Powering off ... ", "");
        gpio_write_off(g_conf);
    }

    if(opt_exit && rocpmd_status != ROCPMD_PROCESS_LIVES)
    {
        gpio_cleanup(g_conf);
        exit(EXIT_SUCCESS);
    }

    try
    {
		ROCPMD->run_daemon(g_conf);
	}
	catch(tdaemon_exception e)
    {
        log_and_report(LOG_CRIT, "Exiting: ", e.what());
        exit(EXIT_FAILURE);
	}
	catch(runtime_error e)
    {
        log_and_report(LOG_CRIT, "Exiting: ", e.what());
        exit(EXIT_FAILURE);
	}
	catch(...)
    {
		unexpected();
	}

    gpio_cleanup(g_conf);

    delete g_conf;

	return 0;
} // end main()
