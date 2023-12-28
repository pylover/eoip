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
#ifndef PROTOCOL_H_
#define PROTOCOL_H_


#include "transport.h"
#include "tunnelset.h"


typedef struct eoip {
    unsigned long packetsin;
    unsigned long packetsout;
} eoip_t;


#undef CAIO_ARG1
#undef CAIO_ARG2
#undef CAIO_ENTITY
#define CAIO_ENTITY eoip
#define CAIO_ARG1 struct tunnelset*
#define CAIO_ARG2 struct transport*
#include <caio/generic.h>  // NOLINT


int
protocol(struct tunnelset *tunnels, struct transport *transport);


#endif  // PROTOCOL_H_
