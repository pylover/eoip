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
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <caio/caio.h>

#include "tunnel.h"


#undef CAIO_ARG1
#undef CAIO_ARG2
#undef CAIO_ENTITY
#define CAIO_ENTITY tunnel
#define CAIO_ARG1 struct transport*
#include <caio/generic.c>  // NOLINT


int
tunnel_print(struct tunnel *t) {
    return printf("%s %s %d\n", t->name, inet_ntoa(t->peer), t->id);
}


void
tunnel_close(struct tunnel *t) {
    if (t->fd == -1) {
        return;
    }

    close(t->fd);
    t->fd = -1;
}


ASYNC
tunnelA(caiotask_t *self, struct tunnel *t,
        struct transport *transport) {
    struct ifreq ifr;
    int fd;
    CAIO_BEGIN(self);

    if (t->fd > -1) {
        ERROR("Tunnel already initialized");
        CAIO_THROW(self, errno);
    }

try:
    errno = 0;
    t->fd = open("/dev/net/tun", O_RDWR | O_NONBLOCK);
    if (t->fd == -1) {
        if (CAIO_MUSTWAITFD()) {
            CAIO_WAITFD(self, t->fd, CAIO_IN | CAIO_OUT | CAIO_ET);
            goto try;
        }
        ERROR("open_tun: /dev/net/tun");
        CAIO_THROW(self, errno);
    }


    /* Create and configure tunnel */
    memset(&ifr, 0x0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
    strncpy(ifr.ifr_name, t->name, IFNAMSIZ);
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        ERROR("socket()");
        CAIO_THROW(self, errno);
    }

    if (ioctl(t->fd, TUNSETIFF, (void *)&ifr) < 0) {
        close(fd);
        ERROR("ioctl-1");
        CAIO_THROW(self, errno);
    }

    ifr.ifr_flags |= IFF_UP;
    ifr.ifr_flags |= IFF_RUNNING;

    if (ioctl(fd, SIOCSIFFLAGS, (void *)&ifr) < 0) {
        close(fd);
        ERROR("ioctl-2");
        CAIO_THROW(self, errno);
    }
    close(fd);

    /* IO loop */
    while (true) {
        // TODO: IO
    }

    CAIO_FINALLY(self);
    if (t->fd > 0) {
        close(t->fd);
    }
}
