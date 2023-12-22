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
#include <string.h>
#include <dirent.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ini.h>
#include <clog.h>

#include "tunnels.h"
#include "options.h"
#include "helpers.h"


static struct tunnel *_tunnels = NULL;
static size_t _tunnelscount = 0;


static void
_print(struct tunnel *t) {
    printf("%s %s %d\n", t->name, inet_ntoa(t->peer), t->id);
}


static void
_reset() {
    if (_tunnels != NULL) {
        free(_tunnels);
    }

    _tunnelscount = 0;
}


static struct tunnel*
_new(const char *name) {
    struct tunnel *new;
    size_t count = _tunnelscount;

    _tunnels = realloc(_tunnels, sizeof(struct tunnel) * (count + 1));
    if (_tunnels == NULL) {
        ERROR("Out of memory");
        return NULL;
    }

    new = _tunnels + count;
    new->id = 0;
    new->peer.s_addr = 0;
    strcpy(new->name, name);
    _tunnelscount++;
    return new;
}


static int
_inihandler(void* , const char* section, const char* name,
        const char* value) {
    struct tunnel *tunnel = NULL;

    if (section == NULL) {
        ERROR("Interface name cannot be null");
        return -1;
    }

    if (strlen(section) > IFNAMSIZ) {
        ERROR("Interface too long");
        return -1;
    }

    if (_tunnelscount == 0) {
        tunnel = _new(section);
    }
    else {
        tunnel = &_tunnels[_tunnelscount - 1];
        if (strcmp(tunnel->name, section)) {
            tunnel = _new(section);
        }
    }

    if (tunnel == NULL) {
        return -1;
    }

    if (strcmp("id", name) == 0) {
        tunnel->id = atoi(value);
    }
    else if (strcmp("dst", name) == 0) {
        if (inet_aton(value, &tunnel->peer) == 0) {
            ERROR("Invalid address: %s", value);
        }
    }
    else {
        ERROR("Invalid config entry: %s", name);
    }

    return 1;
}


static int
_loadfile(const char *filename) {
    INFO("Loading: %s", filename);
    size_t oldcount = _tunnelscount;

    /* Parse the ini file. */
    if (ini_parse(filename, _inihandler, NULL) < 0) {
        ERROR("ini_parse");
        return -1;
    }

    for (;oldcount < _tunnelscount; oldcount++) {
        _print(_tunnels + oldcount);
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

        if (!endswith(filename, ".ini")) {
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
