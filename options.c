// Copyright 2023 vahid mardani
/*
 * This file is part of eoip.
 *  eoip is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  eoip is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with eoip. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <stdlib.h>
#include <stdbool.h>
#include <argp.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "options.h"
#include "clog.h"
#include "manifest.h"


#define CONFIGPATH_DEFAULT "/etc/eoip.d/"


struct options options = {
    /* Common */
    .command = CMD_LIST,
    .verbosity = CLOG_DEBUG,
    .configpath = CONFIGPATH_DEFAULT,
};


const char *argp_program_version = EOIP_VERSION;
const char *argp_program_bug_address = "http://github.com/pylover/eoip";
static char doc[] = "\nSimple Virtual Private Network";
static char args_doc[] = "list\n"
    "start [TUNNEL]";


/* Options definition */
static struct argp_option opts[] = {
    {
        "verbosity",
        'v',
        "LEVEL",
        0,
        "Verbosity level: 0-5. default: 4"
    },
    {
        "configpath",
        'c',
        "PATH",
        0,
        "Configuration path, default: " CONFIGPATH_DEFAULT " (if exists)"
    },
    {
        "bind",
        'b',
        "ADDRESS",
        0,
        "Bind address, default: 0"
    },
    {NULL}
};


static enum command
command_parse(char *cmd) {
    if (cmd == NULL) {
        goto unknown;
    }

    if (strcmp(cmd, "list") == 0) {
        return CMD_LIST;
    }
    else if (strcmp(cmd, "start") == 0) {
        return CMD_START;
    }

unknown:
    return CMD_UNKNOWN;
}


/* Parse a single option. */
static int
parse_opt(int key, char *arg, struct argp_state *state) {
    switch (key) {
        /* Common */
        case 'v':
            options.verbosity = (unsigned char) atoi(arg);
            if (options.verbosity > 5) {
                goto reject;
            }
            break;

        case 'b':
            if (inet_aton(arg, &options.bind)) {
                ERROR("Invalid bind address: %s", arg? arg: "(null)");
                goto reject;
            }
            break;

        case 'c':
            options.configpath = arg;
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                options.command = command_parse(arg);
                if (options.command == CMD_UNKNOWN) {
                    goto reject;
                }
                goto success;
            }
            else if ((state->arg_num == 1) &&
                    (options.command != CMD_START)) {
                /* Too many arguments. */
                goto reject;
            }
            options.argv[state->arg_num - 1] = arg;
            options.argc++;
            break;

        // /* Others */
        // case ARGP_KEY_NO_ARGS:
        //     goto reject;

        // // case ARGP_KEY_END:
        // //     break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

success:
    return 0;

reject:
    // exit the process
    argp_usage(state);
    return -1;
}


static struct argp argp = {
    .options = opts,
    .parser = parse_opt,
    .args_doc = args_doc,
    .doc = doc,
    .children = NULL,
    .help_filter = NULL,
    .argp_domain = NULL,
};


int
options_parse(int argc, char **argv) {
    int e = argp_parse(&argp, argc, argv, 0, 0, 0);
    if (e) {
        ERROR("Cannot parse arguments");
        return -1;
    }

    return 0;
}

