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
#include <net/if.h>
#include <string.h>
#include <dirent.h>
#include <ini.h>

#include <clog.h>

#include "tunnels.h"
#include "options.h"
#include "helpers.h"


static struct tunnel *_tunnels = NULL;
static size_t _tunnelscount = 0;


static void
_reset() {
    if (_tunnels != NULL) {
        free(_tunnels);
    }

    _tunnelscount = 0;
}


static int
_inihandler(void* user, const char* section, const char* name,
        const char* value) {
    // struct tunnel *tunnel = (struct tunnel*)user;

    DEBUG("%s %s %s", section, name, value);
    // return 0;  /* unknown section/name, error */
    return 1;
}


static int
_loadfile(const char *filename) {
    struct tunnel tunnel = {
        .name = "\0",
        .peer = {0x0},
        .id = 0,
    };

    DEBUG("Loading: %s", filename);

    /* Parse the ini file. */
    if (ini_parse(filename, _inihandler, &tunnel) < 0) {
        ERROR("ini_parse");
        return -1;
    }

    return 0;
}


int
tunnels_load() {
    DIR *dp;
    struct dirent *ep;
    char filename[1024];

    dp = opendir(options.configpath);
    if (dp == NULL) {
        ERROR("Couldn't open the directory: %s", options.configpath);
        return -1;
    }

    _reset();
    INFO("Looking %s for configuration files", options.configpath);
    while ((ep = readdir(dp)) != NULL) {
        joinpath(filename, options.configpath, ep->d_name);
        if (!isfile(filename)) {
            continue;
        }

        if (_loadfile(filename)) {
            WARN("Cannot load %s", filename);
        }
    }

    closedir(dp);
    return 0;
}


int
tunnels_list() {
    if (tunnels_load()) {
        return -1;
    }
    return 0;
}
