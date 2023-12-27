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

#include "tunnel.h"


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


int
tunnel_open(struct tunnel *t) {
    struct ifreq ifr;
    int fd;

    if (t->fd > -1) {
        return 0;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        ERROR("socket()");
        return -1;
    }

    t->fd = open("/dev/net/tun", O_RDWR);
    if (t->fd < 0) {
        ERROR("open_tun: /dev/net/tun");
        goto failed;
    }

    memset(&ifr, 0x0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
    strncpy(ifr.ifr_name, t->name, IFNAMSIZ);

    if (ioctl(t->fd, TUNSETIFF, (void *)&ifr) < 0) {
        ERROR("ioctl-1");
        goto failed;
    }

    ifr.ifr_flags |= IFF_UP;
    ifr.ifr_flags |= IFF_RUNNING;

    if (ioctl(fd, SIOCSIFFLAGS, (void *)&ifr) < 0) {
        ERROR("ioctl-2");
        goto failed;
    }
    close(fd);
    return 0;

failed:
    close(fd);
    if (t->fd > 0) {
        close(t->fd);
    }
    return -1;
}
