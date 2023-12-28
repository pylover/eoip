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
#include <caio/caio.h>

#include "tunnel.h"
#include "tunnelset.h"
#include "manifest.h"
#include "protocol.h"
#include "transport.h"


#undef CAIO_ARG1
#undef CAIO_ARG2
#undef CAIO_ENTITY
#define CAIO_ENTITY eoip
#define CAIO_ARG1 struct tunnelset*
#define CAIO_ARG2 struct transport*
#include <caio/generic.c>  // NOLINT


static ASYNC
eoipA(struct caio_task *self, struct eoip *eoip, struct tunnelset *tunnels,
        struct transport *transport) {
    static int i;
    CAIO_BEGIN(self);

    /* Initialize transport */
    AWAIT(self, transport, transport_initA, transport);

    /* Openning all tunnel devices */
    for (i = 0; i < tunnels->count; i++) {
        AWAIT(self, tunnel, tunnelA, tunnels->first + i, transport);
    }
    CAIO_FINALLY(self);
}


int
protocol(struct tunnelset *tunnels, struct transport *transport) {
    int ret;
    static struct eoip *eoip = NULL;

    if (eoip == NULL) {
        eoip = malloc(sizeof(struct eoip));
        if (eoip == NULL) {
            ERROR("Out of memory");
            return -1;
        }
    }

    ret = eoip_forever(eoipA, eoip, tunnels, transport,
            tunnels->count + 3, CAIO_SIG);

    if (eoip) {
        free(eoip);
    }
    return ret;
}
