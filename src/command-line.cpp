#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>

#include <iostream>

#include "command-line.h"

using namespace std;

cmdline_options::cmdline_options(int argc, char **argv) :
    power_off_flag(0),
    no_daemon_mode_flag(0),
    no_websocket_flag(0),
    power_level_flag(0),
    power_level_raw_flag(0),
    verbose_flag(0),
    help_flag(0),
    config_path("/etc/powerd.conf")
{
    int c;

    while (1)
    {

      static struct option long_options[] =
        {
          {"config-path:",    required_argument,   0,                     'c'},
          {"power-off",       no_argument,         &power_off_flag,       'p' },
          {"no_daemon-mode",  no_argument,         &no_daemon_mode_flag,  'd' },
          {"no_websocket",    no_argument,         &no_websocket_flag,    'w'},
          {"power-level",     no_argument,         &power_level_flag,     'l'},
          {"power-level-raw", no_argument,         &power_level_raw_flag, 'r'},
          {"verbose",         no_argument,         &verbose_flag,         'v' },
          {"help",            no_argument,         &help_flag,            'h' },
          {0, 0, 0, 0}
        };

      int option_index = 0;

      c = getopt_long (argc, argv, "c:dpwlrh",
                       long_options, &option_index);

      // Detect the end of the options.

      if (c == '?' || c == ':')
      {
          exit(1);
      }

      if (c == -1)
      {
          break;
      }

      switch (c)
        {
        case 0:
          // If this option set a flag, do nothing else now.
          if (long_options[option_index].flag != 0)
            break;
          /*
          printf ("long option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          */
          break;

        case 'c':
          //printf ("short option -c with value `%s'\n", optarg);
          config_path = optarg;

          break;

        case 'p':
          //puts ("short option -p\n");
          power_off_flag = true;
          break;

        case 'd':
          //puts ("short option -d\n");
          no_daemon_mode_flag = true;
          break;

        case 'h':
          //puts ("short option -h\n");
          help_flag = true;
          break;

        case 'w':
          //puts ("short option -w\n");
          no_websocket_flag = true;
          break;

        case 'l':
          //puts ("short option -l\n");
          power_level_flag = true;
          break;

        case 'r':
          //puts ("short option -r\n");
          power_level_raw_flag = true;
          break;

        case '?':
          // getopt_long already printed an error message.
          break;

        default:
          abort ();
        }
    }

    /*FIXME: Dead code?
    // Instead of reporting ‘--verbose’
    // and ‘--brief’ as they are encountered,
    // we report the final status resulting from them.
    if (power_off_flag)
    {
        puts ("power-off flag is set");
    }

    if (no_daemon_mode_flag)
    {
        puts ("no_daemon-mode flag is set");
    }

    if (no_websocket_flag)
    {
        puts ("no_websocket flag is set");
    }

    if (power_level_flag)
    {
        puts ("power-level flag is set");
    }

    if (power_level_raw_flag)
    {
        puts ("power-level-raw flag is set");
    }

    if (verbose_flag)
    {
        puts ("verbose flag is set");
    }

    if (help_flag)
    {
        puts ("help flag is set");
    }

    // Print any remaining command line arguments (not options).
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        putchar ('\n');
    }
    */
}

bool cmdline_options::is_power_off()
{
    return power_off_flag;
}

bool cmdline_options::is_no_daemon_mode()
{
    return no_daemon_mode_flag;
}

bool cmdline_options::is_no_websocket()
{
    return no_websocket_flag;
}

bool cmdline_options::is_power_level()
{
    return power_level_flag;
}

bool cmdline_options::is_power_level_raw()
{
    return power_level_raw_flag;
}

bool cmdline_options::is_verbose()
{
    return verbose_flag;
}

bool cmdline_options::is_help()
{
    return help_flag;
}

string cmdline_options::get_config_path()
{
    return config_path;
}
