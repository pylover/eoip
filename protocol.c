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
#include <caio/generic.c>


static ASYNC
eoipA(struct caio_task *self, struct eoip *eoip) {
    CAIO_BEGIN(self);

    AWAIT(self, transport, transportA, &eoip->transport);
    CAIO_FINALLY(self);
}


int
protocol(struct tunnelset *tunnels) {
    int ret;
    static struct eoip *eoip = NULL;

    if (eoip == NULL) {
        eoip = malloc(sizeof(struct eoip));
        if (eoip == NULL) {
            ERROR("Out of memory");
            return -1;
        }
    }

    eoip->tunnels = tunnels;
    ret = eoip_forever(eoipA, eoip, tunnels->count + 3, CAIO_SIG);

    if (eoip) {
        free(eoip);
    }
    return ret;
}
