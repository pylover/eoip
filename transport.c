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
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <clog.h>

#include "transport.h"
#include "options.h"


static int sockfd = -1;


int
transport_create() {
    int fd;
    const int enable = 1;
    const int bsize = 262144;

    fd = socket(PF_INET, SOCK_RAW, 47);
    if (fd == -1) {
        if (errno == EPERM) {
            ERROR("You need root privileges or CAP_NET_RAW capability to run "
                    "this program");
        } else {
            ERROR("raw socket()");
        }

        goto failed;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bsize, sizeof(bsize)) < 0) {
        ERROR("setsockopt(RCVBUF)");
        goto failed;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bsize, sizeof(bsize)) < 0) {
        ERROR("setsockopt(SNDBUF)");
        goto failed;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        ERROR("setsockopt(SO_REUSEADDR)");
        goto failed;
    }

    /* Bind */
    if (options.bind.s_addr && bind(fd, (struct sockaddr*) &options.bind,
                sizeof(struct sockaddr))) {
        ERROR("bind(%s:%d)", inet_ntoa(options.bind));
        goto failed;
    }

    sockfd = fd;
    return 0;

failed:
    if (fd > -1) {
        close(fd);
        fd = -1;
    }
    return -1;
}
