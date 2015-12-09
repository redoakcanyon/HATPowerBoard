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
 
   File name: tdaemon.cpp
   Date:      2006-08-08 14:22
   Author:    Kristjan Runarsson

   -----------------------------------------------------------------------------
 */

#include "tdaemon.h"
#include <stdexcept>
#include <signal.h>
#include <sstream>

#include <syslog.h>

#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

tdaemon::tdaemon(string dameonName, string lock_file_name, int daemon_flags):
                 _name(dameonName),
                 _work_dir("/tmp"),
                 _daemon_flags(daemon_flags),
                 _pid(getpid()),
                 _lockfile_name(lock_file_name),
                 _lockfile_dir(LOCKDIR)
{
    stringstream lock_path_stream;

    lock_path_stream << _lockfile_dir << "/" << lock_file_name;

    _lockfile_path = lock_path_stream.str();
}

tdaemon::~tdaemon()
{
    cleanup();
}

int tdaemon::run_daemon(config *conf)
{
    daemonize();
    daemon_main(conf);
    return 0;
}

void tdaemon::cleanup()
{
    unlock();
    close(lock_fd);
    unlink(_lockfile_path.c_str());
}

//virtual int tdaemon::daemon_main(config *conf) = 0;

void tdaemon::daemonize()
{
    if(!(_daemon_flags & NO_SUMSK))
    {
        set_umask();
    }
    
    if(!(_daemon_flags & NO_DTTY))
    {
        detach_terminal();
    }
    
    if(!(_daemon_flags & NO_ISLOG))
    {
        // Make this call first because other methods in this class call syslog. 
        init_syslog();
    }

    if(!(_daemon_flags & NO_CROOT))
    {
        change_work_dir();
    }
        
    if(_daemon_flags & CLOSE_ALL_FD)
    {
        fd_close();
    }
    else
    {
        fd_close(3);
    }
    
    if(!(_daemon_flags & NO_FTNULL))
    {
        fd_to_dev_null();
    }

    if((_daemon_flags & SINGLETON))
    {
        if(is_running())
        {
            exit(EXIT_FAILURE);
        }
    }
}

void tdaemon::set_umask()
{
    umask(0);
}

void tdaemon::detach_terminal()
{
    pid_t   pid;
    struct  sigaction sa;

    // Become a session leader to lose the controlling terminal.
    if((pid=fork())<0) //Fork a child process.
    {
        stringstream msg;
        msg << _name << " (pid: " << _pid << ") was unable to fork." << endl;
        syslog(LOG_CRIT, msg.str().c_str());

        throw fork_exception("A call to fork() failed.");
    }
    else if(pid!=0) // The parent exits here.
    {
        exit(EXIT_SUCCESS);
    }

    setsid(); // Create a new session.

    sa.sa_handler=SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;

    if(sigaction(SIGHUP, &sa, NULL)<0)
    {
        throw sighup_exception("I can't ignore SIGHUP.");
    }

    // Fork a second child to guarantee the daemon is not a session leader
    // which means that under System V it can't acquire a controlling terminal.
    if((pid=fork())<0) //Fork a child process.
    {
        throw fork_exception("A call to fork() failed.");
    }
    else if(pid!=0) // The parent exits here.
    { 
        exit(EXIT_SUCCESS);
    }

    _pid=getpid();
    
}

void tdaemon::change_work_dir()
{
    chdir(_work_dir.c_str());
}

void tdaemon::fd_close(unsigned int i)
{
    int res = -1;
    struct rlimit rl;

    // Get maximum number of file descriptors.
    if(i == 0)
    {
        res = getrlimit(RLIMIT_NOFILE, &rl);
        if(res  == -1)
        {
            throw fd_max_exception("Unable to obtain maximum file limit.");
        }
    }

    if(i == 0)
    {
        // Close all open file descriptors.
        if(rl.rlim_max==RLIM_INFINITY)
        {
            rl.rlim_max=1024;
        }
    }
    else
    {
        // Close only stdin, stdout and stderr.
        rl.rlim_max = 3;
    }

    // Close the file descriptors.
    for(i=0; i<rl.rlim_max; i++)
    {
        close(i);
    }
}

void tdaemon::fd_to_dev_null()
{
    // Having closed all the filedescriptors open will return the lowest 
    // avalable one which is '0'.
    open("/dev/null", O_RDWR); 
    fcntl(0,F_DUPFD,1);  // Apparently dup & dup2 are redundant.
    fcntl(0,F_DUPFD,2);
}

void tdaemon::init_syslog()
{
    // Strictly speaking this is optional since openlog gets called the first
    // time syslog is called. Calling it here is cleaner since it prefixes an
    // ident to the syslog messages.
    openlog(get_name().c_str(), LOG_CONS, LOG_DAEMON); 
}

int tdaemon::lock()
{
    struct flock fl;

    fl.l_type   = F_WRLCK;
    fl.l_start  = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len    = 0;

    return (fcntl(lock_fd, F_SETLK, &fl));
}

int tdaemon::unlock()
{
    struct flock fl;

    fl.l_type = F_UNLCK;
    fl.l_start  = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len    = 0;

    return (fcntl(lock_fd, F_SETLK, &fl));
}

bool tdaemon::is_running()
{
    char buf[16];

    lock_fd = open(_lockfile_path.c_str(), O_RDWR | O_CREAT, LOCKMODE);

    if(lock_fd < 0)
    {
        syslog(LOG_CRIT, "Cannot open %s: %s", _lockfile_path.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(lock() < 0)
    {
        if(errno == EACCES || errno == EAGAIN)
        {
            close(lock_fd);
            syslog(LOG_CRIT, "Lockfile  %s is already locked.", _lockfile_path.c_str());
            return true;
        }

        syslog(LOG_CRIT, "Cannot lock  %s: %s", _lockfile_path.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    ftruncate(lock_fd, 0);

    sprintf(buf, "%ld", (long)getpid());
    write(lock_fd, buf, strlen(buf)+1);
    
    return false;
}

void tdaemon::check_work_dir(string& work_dir)
{
    char work_dir_buf[2048];
    
    getcwd(work_dir_buf,sizeof(work_dir_buf));
    work_dir=work_dir_buf;
}

string tdaemon::get_name()
{
    return _name;
}

int tdaemon::get_pid()
{
    return _pid;
}

string tdaemon::get_lockfile_name()
{
    return _lockfile_name;
}

string tdaemon::get_lockfile_path()
{
    return _lockfile_path;
}

