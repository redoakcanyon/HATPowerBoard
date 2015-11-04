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

   File name: tdaemon.h
   Date:      2006-08-08 14:29
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
*/

#include <iostream>
#include <string>
#include <stdexcept>

//Requisite C libraries. FIXME check, not all of them will really be needed.
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef __TDAEMON_H__
#define __TDAEMON_H__

// The Class initialization flag 0x10000 to 0x100008000.
#define SINGLETON 0x100    /* Enables creation of a lockfile to ensure the  
                           daemon is a singleton.*/

// Debug flags 0x1 to 0x8000, most daemons won't have any reason use them.
#define NO_SUMSK        0x1   // Do not set UMASK.
#define NO_DTTY         0x2    // Disable detachment from TTY (i.e. fork() the daemon)
#define NO_ISLOG        0x4    // Disabe initializaton of syslog
#define NO_CROOT        0x8   // Do not changed the daemon's working directory.
#define CLOSE_ALL_FD    0x10    // Disable closing of file descriptors
#define NO_FTNULL       0x20    // Disable attachment of FDs 0,1,2 to /dev/null

#define LOCKDIR  "/var/run"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

class config;

using namespace std;
/**  \class tdaemon
 * \brief A daemon base class.
 *
 * This simple daemon base class provides all the basic tasks a daemon has to 
 * do peform. The intention is that this class be extended by the developer.
 */

//class tdaemon_exception: public Exception{
//}

class tdaemon{
    private:
        int lock_fd;

    public:
        /** Constructor.*/    
        tdaemon(std::string daemonName, std::string lock_file_name, int daemon_flags);
    
        /** Destructor.*/
        ~tdaemon();
    
        /** This Method is called from main. It calls daemonize and Work(), thus
         * starting the daemon.*/
        int run_daemon(config *conf);
        
        /** The working logic of the daemon should be implemented in this method.*/
        virtual int daemon_main(config *conf) = 0;
    
        /** Checks the working directory using the getwd() function form the standard
         * C-library rather than just retruning the value of '_work_dir'.*/
        void check_work_dir(string& sWorkDir);
    
        /** Attempts to open a lock file and such that it can be: read and
         * written to by it's owner, read by it's owner's group and read by others.
         * It then attempts to lock the file if this fails true is returned else
         * the function truncates the lockfile, writes the daemon's PID into it
         * and returns false.
         */
        bool is_running();
    
        /**
         */
        //Accessors.
        /**Accessor.*/
        string get_name();
        /**Accessor.*/
        int get_pid();
        /**Accessor.*/
        string get_lockfile_name();
        /**Accessor.*/
        string get_lockfile_path();
        /**Accessor.*/
        string get_work_dir();
        
    
    protected:
    
        /** The name of this daemon.*/
        string _name;
    
        /** The daemon's working directory.*/
        string _work_dir;
    
        /** Initialization flags for the daemon defined in tdaemon.h.*/
        unsigned int _daemon_flags;
        
        /** The pid of the second child process (the actual daemon).*/
        int _pid;
    
        /** The name of the lockfile.*/
        string _lockfile_name;
    
        /** Lockfile default location, hardwired to /var/run.*/
        string _lockfile_dir;

        /** Lockfile directory and name combined into a path */
        string _lockfile_path;
    
        /** Change the File Mode Mask (umask) to ensure ensure files created by 
          * the daemon can be written and read properly.*/
        void set_umask();
        
        /** The standard daemonization method. Implement the daemonization logic
         * using the utility functions provided by this class.*/
        virtual void daemonize(); 
    
        /** Become a session leader to lose any controlling terminal and ensure that
         * future opens won't allocate controlling terminals.*/
        void detach_terminal();
        
        /** Change working directory to '/' to avoid blocking a umount.*/
        void change_work_dir();
        
        /** Close all open file descripteors inherited form the parent.*/
        void fd_close(unsigned int i = 0);
        
        /** Attatch file descriptors 0, 1 and 2 to /dev/null to prevent unwanted 
          * reads and writes to STDOUT/STDIN/STDERR.*/
        void fd_to_dev_null();
    
        /** Initialize the log facility.*/
        void init_syslog();
        
        /** Create lock file to ensure the daemon is a singleton.*/
        int lock();
}; // end class tdaemon

//Base exception class for tdaemon.
class tdaemon_exception : public exception
{
public:
    explicit tdaemon_exception(const string& what):
                                   m_what(what){
    }

    virtual ~tdaemon_exception() throw() {}

    virtual const char * what() const throw()
    {
        return m_what.c_str();
    }

private:
    string m_what;
};

class fork_exception : public tdaemon_exception{
public:
    fork_exception(const string& what) : tdaemon_exception(what){}
    virtual ~fork_exception() throw() {}
};

class sighup_exception : public tdaemon_exception{
public:
    sighup_exception(const string& what) : tdaemon_exception(what){}
    virtual ~sighup_exception() throw() {}
};

class fd_max_exception : public tdaemon_exception{
public:
    fd_max_exception(const string& what) : tdaemon_exception(what){}
    virtual ~fd_max_exception() throw() {}
};

#endif
