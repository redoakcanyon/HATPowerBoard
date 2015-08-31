/*
 * File name: tdaemon.cpp
 * Date:	  2006-08-08 10:32
 * Author:	Kristján Rúnarsson
 */

#include "tdaemon.h"
#include <stdexcept>
#include <signal.h>
#include <sstream>

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
}

int tdaemon::run_daemon(config *conf)
{
	daemonize();
	daemon_main(conf);
	return 0;
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
		
	if(!(_daemon_flags & NO_CFDSC))
    {
		fd_close();
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
		//sleep(10); // DEBUG
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
		//sleep(10); // DEBUG
		exit(EXIT_SUCCESS);
	}

	_pid=getpid();
	
}

void tdaemon::change_work_dir()
{
	chdir(_work_dir.c_str());
}

void tdaemon::fd_close()
{
	unsigned int i;
	struct rlimit rl;

	// Get maximum number of file descriptors.
	if(getrlimit(RLIMIT_NOFILE, &rl)<0)
    {
		throw fd_max_exception("Unable to obtain maximum file limit.");
	}
	
	// Close all open file descriptors.
	if(rl.rlim_max==RLIM_INFINITY)
    {
		rl.rlim_max=1024;
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

bool tdaemon::is_running()
{
    char buf[16];

    lock_fd = open(_lockfile_path.c_str(), O_RDWR | O_CREAT, LOCKMODE);

    syslog(LOG_CRIT, "ir1");

    if(lock_fd < 0)
    {
        syslog(LOG_CRIT, "Cannot open %s: %s", _lockfile_path.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_CRIT, "ir2");

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
	syslog(LOG_CRIT,work_dir_buf);
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

