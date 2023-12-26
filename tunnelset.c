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
// #include <unistd.h>
// #include <string.h>
#include <dirent.h>
// #include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//
#include <ini.h>
#include <clog.h>
//
// #include "tunnels.h"
#include "options.h"
#include "helpers.h"
#include "tunnel.h"
#include "tunnelset.h"


static struct tunnel*
_new(struct tunnelset *tunnels, const char *name, const char *filename) {
    int i;
    struct tunnel *new;
    size_t count = tunnels->count;

    /* Ensure the tunnel is not exists yet */
    for (i = 0; i < count; i++) {
        if (strcmp(name, tunnels->first[i].name) == 0) {
            ERROR("Duplicate tunnel name: %s", name);
            return NULL;
        }
    }

    tunnels->first = realloc(tunnels->first,
            sizeof(struct tunnel) * (count + 1));
    if (tunnels->first == NULL) {
        ERROR("Out of memory");
        return NULL;
    }

    new = tunnels->first + count;
    new->id = 0;
    new->fd = -1;
    new->peer.s_addr = 0;
    strcpy(new->name, name);
    strcpy(new->filename, filename);
    tunnels->count++;
    return new;
}


static int
_inihandler(struct tunnelset *tunnels, const char* section, const char* name,
        const char* value) {
    struct tunnel *tunnel = NULL;
    const char *filename = "foo.ini";

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
        // TODO: set filename by modifying ini.c
        tunnel = _new(tunnels, section, filename);
        if (tunnel == NULL) {
            return 0;
        }

        return 1;
    }

    if (tunnels->count == 0) {
        return 0;
    }

    /* Updating tunnel's attributes */
    tunnel = &tunnels->first[tunnels->count - 1];
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
_conflicts(struct tunnelset *tunnels) {
    int i;
    int j;
    struct tunnel *ti;
    struct tunnel *tj;

    for (i = 0; i < tunnels->count; i++) {
        ti = tunnels->first + i;

        for (j = 0; j < tunnels->count; j++) {
            if (i == j) {
                continue;
            }
            tj = tunnels->first + j;

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
_loadfile(struct tunnelset *tunnels, const char *filename,
        const char *basename) {
    INFO("Loading: %s", filename);
    size_t oldcount = tunnels->count;

    /* Parse the ini file. */
    if (ini_parse(filename, (ini_handler)_inihandler, (void *)basename)) {
        ERROR("ini_parse");
        goto failed;
    }

    /* Check consistency */
    if (_conflicts(tunnels)) {
        goto failed;
    }

    return 0;

failed:
    if (oldcount < tunnels->count) {
        tunnels->first = realloc(tunnels->first,
                sizeof(struct tunnel) * oldcount);
        tunnels->count = oldcount;
    }
    return -1;
}


void
tunnelset_dispose(struct tunnelset *tunnels) {
    if (tunnels->first != NULL) {
        free(tunnels->first);
    }

    tunnels->count = 0;
}


int
tunnelset_load(struct tunnelset *tunnels) {
    DIR *dp;
    struct dirent *ep;
    char filename[1024];

    dp = opendir(options.configpath);
    if (dp == NULL) {
        ERROR("Couldn't open the directory: %s", options.configpath);
        return -1;
    }

    tunnelset_dispose(tunnels);
    INFO("Looking %s for configuration files ...", options.configpath);
    while ((ep = readdir(dp)) != NULL) {
        joinpath(filename, options.configpath, ep->d_name);
        if (!isfile(filename)) {
            continue;
        }

        if (!endswith(filename, ".ini")) {
            continue;
        }

        if (_loadfile(tunnels, filename, ep->d_name)) {
            WARN("Cannot load %s", filename);
        }
    }

    closedir(dp);
    return 0;
}


int
tunnelset_print(struct tunnelset *tunnels) {
    int i;

    for (i = 0; i < tunnels->count; i++) {
        tunnel_print(tunnels->first + i);
    }

    return 0;
}


void
tunnelset_closeall(struct tunnelset *tunnels) {
    int i;

    for (i = 0; i < tunnels->count; i++) {
        tunnel_close(tunnels->first + i);
    }
}


int
tunnelset_openall(struct tunnelset *tunnels) {
    int i;

    for (i = 0; i < tunnels->count; i++) {
        if (tunnel_open(tunnels->first + i)) {
            goto failed;
        }
    }

    return 0;
failed:
    tunnelset_closeall(tunnels);
    return -1;
}
