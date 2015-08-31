#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#include <string>

class cmdline_options
{
    private:
        int power_off_flag;
        int no_daemon_mode_flag;
        int no_websocket_flag;
        int power_level_flag;
        int power_level_raw_flag;
        int verbose_flag;
        int help_flag;
        std::string config_path;

    public:
        cmdline_options(int argc, char **argv);

        bool is_power_off();
        bool is_no_daemon_mode();
        bool is_no_websocket();
        bool is_power_level();
        bool is_power_level_raw();
        bool is_verbose();
        bool is_help();
        std::string get_config_path();
};

#endif
