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
    printf("%s %s %d %s\n", t->name, inet_ntoa(t->peer), t->id, t->filename);
}


static struct tunnel*
_new(const char *name, const char *filename) {
    int i;
    struct tunnel *new;
    size_t count = _tunnelscount;

    /* Ensure the tunnel is not exists yet */
    for (i = 0; i < count; i++) {
        if (strcmp(name, _tunnels[i].name) == 0) {
            ERROR("Duplicate tunnel name: %s", name);
            return NULL;
        }
    }

    _tunnels = realloc(_tunnels, sizeof(struct tunnel) * (count + 1));
    if (_tunnels == NULL) {
        ERROR("Out of memory");
        return NULL;
    }

    new = _tunnels + count;
    new->id = 0;
    new->peer.s_addr = 0;
    strcpy(new->name, name);
    strcpy(new->filename, filename);
    _tunnelscount++;
    return new;
}


static int
_inihandler(const char *filename , const char* section, const char* name,
        const char* value) {
    struct tunnel *tunnel = NULL;

    if (section == NULL) {
        ERROR("Interface name cannot be null");
        return 0;
    }

    if (strlen(section) > IFNAMSIZ) {
        ERROR("Interface too long");
        return 0;
    }

    if ((name == NULL) && (value == NULL)) {
        /* New section */
        tunnel = _new(section, filename);
        if (tunnel == NULL) {
            return 0;
        }

        return 1;
    }

    if (_tunnelscount == 0) {
        return 0;
    }

    /* Updating tunnel's attributes */
    tunnel = &_tunnels[_tunnelscount - 1];
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
_conflicts() {
    int i;
    int j;
    struct tunnel *ti;
    struct tunnel *tj;

    for (i = 0; i < _tunnelscount; i++) {
        ti = _tunnels + i;

        for (j = 0; j < _tunnelscount; j++) {
            if (i == j) {
                continue;
            }
            tj = _tunnels + j;

            if ((ti->id == tj->id) && (ti->peer.s_addr == tj->peer.s_addr)) {
                ERROR("Identical tunnels found: %s:%s and %s:%s are both "
                        "defined as  dst=%s and id=%d",
                        ti->filename, ti->name, tj->filename, tj->name,
                        inet_ntoa(ti->peer), ti->id);
                return -1;
            }
        }
    }

    return 0;
}


static int
_loadfile(const char *filename, const char *basename) {
    INFO("Loading: %s", filename);
    size_t oldcount = _tunnelscount;

    /* Parse the ini file. */
    if (ini_parse(filename, (ini_handler)_inihandler, (void *)basename)) {
        ERROR("ini_parse");
        goto failed;
    }

    /* Check consistency */
    if (_conflicts()) {
        goto failed;
    }

    return 0;

failed:
    if (oldcount < _tunnelscount) {
        _tunnels = realloc(_tunnels, sizeof(struct tunnel) * oldcount);
        _tunnelscount = oldcount;
    }
    return -1;
}


void
tunnels_dispose() {
    if (_tunnels != NULL) {
        free(_tunnels);
    }

    _tunnelscount = 0;
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

    tunnels_dispose();
    INFO("Looking %s for configuration files ...", options.configpath);
    while ((ep = readdir(dp)) != NULL) {
        joinpath(filename, options.configpath, ep->d_name);
        if (!isfile(filename)) {
            continue;
        }

        if (!endswith(filename, ".ini")) {
            continue;
        }

        if (_loadfile(filename, ep->d_name)) {
            WARN("Cannot load %s", filename);
        }
    }

    closedir(dp);
    return 0;
}


int
tunnels_list() {
    int i;

    for (i = 0; i < _tunnelscount; i++) {
        _print(_tunnels + i);
    }

    return 0;
}
