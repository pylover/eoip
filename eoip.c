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
#include <stdio.h>
#include <stdlib.h>

#include <clog.h>

#include "options.h"
#include "tunnels.h"
#include "manifest.h"


int
main(int argc, char **argv) {
    int ret = EXIT_SUCCESS;

    /* Parse command line arguments */
    if (options_parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    /* Init done */
    clog_verbosity = options.verbosity;
    INFO("MiktoTik EoIP v%s", EOIP_VERSION);

    switch (options.command) {
        case CMD_LIST:
            ret = tunnels_list();
            break;

        case CMD_START:
            DEBUG("Start, argc: %d", options.argc);
            // TODO: Start daemon
            break;

        default:
            ERROR("Unknown command: %s", argv[1]);
    }

    return ret;
}
