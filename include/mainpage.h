/** \mainpage Code documentation for the 'amsd' daemon
*
* \section section Project synopsis
* This is a simple daemon that listenes on a SOCK_STREAM/AF_INET <i>(TCP/IP to the uninitiated)</i> socket for MTP messages from Ericsson AXE switches. On recipt it will identify these messages, parse them as needed and then store them in an Oracle database. The daemon is written in C++ and uses proprietary Oracle DB access libraries. It should compile properly on most flavors of Linux and Unix assuming the proprietary Oracle libraries are available for the platform in question. I have tried to keep the business logic seperate from the code that calls platform specific system calls i.e. the daemonization and the networking code. To modify this for use as a system service for a Windows NT derived operating system will require rewriting those portions of the code for Widows sockets and replacing the Linux/Unix daemonization code with the code needed to enable it to run as a Windows system service.
*
*/

//Fake *.h file for Doxygen.
