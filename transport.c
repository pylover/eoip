#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <clog.h>

#include "transport.h"


// static int sockfd = -1;


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

        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bsize, sizeof(bsize)) < 0) {
	    ERROR("setsockopt(RCVBUF)");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bsize, sizeof(bsize)) < 0) {
        DEBUG("setsockopt(SNDBUF)");
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        ERROR("setsockopt(SO_REUSEADDR)");
    }

    // /* Bind */
    // if (bind(thr_rx_data.raw_socket, (struct sockaddr *) &serv_addr,
    //     sizeof(serv_addr)) < 0) {
    //     ERROR("bind(%s:%d)", inet_ntoa(options.bind);
    // }
    //
    return 0;
}
