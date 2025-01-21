#include "socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define MAX_PORT_NUMBER 65535

struct socketStruct {
	int fd;
	uint32_t boundPort;
	unsigned short maxConnectionQueue;
};

socket_t *createSocket(config_t *conf) {
	if (!conf) {
		return NULL;
	}

	socket_t *out = malloc(sizeof(socket_t));
	if (!out) {
		return NULL;
	}

	out->fd = socket(conf->ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (out->fd < 0) {
		perror("Could not create socket");
		free(out);
		return NULL;
	}

	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = conf->ipv6 ? AF_INET6 : AF_INET;
	addr.sin_port = htons(conf->webserverPort);

	uint32_t port = conf->webserverPort;
	if (-1 == bind(out->fd, (struct sockaddr *) &addr, sizeof(addr))) {
		if (errno == 98) {
			if (!conf->findNextAvailablePort) {
				printf("Port already in use.\n");
				exit(1);
			} else {
				for (port += 1; port <= MAX_PORT_NUMBER; port++) {
					printf("Port %d in use, trying port %d...\n", port - 1, port);
					addr.sin_port = htons(port);

					if (-1 == bind(out->fd, (struct sockaddr *) &addr, sizeof(addr))) {
						if (errno == 98) {
							continue;
						} else {
							perror("Could not bind socket");
							exit(1);
						}
					} else {
						break;
					}
				}

				if (port > MAX_PORT_NUMBER) {
					printf("Could not find an available port\n");
					exit(1);
				}
			}
		} else {
			perror("Could not bind socket");
			exit(1);
		}
	}
	out->boundPort = port;

	if (-1 == listen(out->fd, conf->connectionQueueMaxSize)) {
		perror("Could not begin listening for connections");
		free(out);
		return NULL;
	}
	out->maxConnectionQueue = conf->connectionQueueMaxSize;

	return out;
}

unsigned short socketGetBoundPort(socket_t *sock) {
	if (!sock) {
		return 0;
	}

	return sock->boundPort;
}

clientConnection_t *socketAcceptConnection(socket_t *sock) {
	if (!sock) {
		return NULL;
	}

	struct sockaddr_in addr;

	socklen_t addrLen;
	int fd = accept(sock->fd, (struct sockaddr *) &addr, &addrLen);
	if (fd < 0) {
		return NULL;
	}

	return createClientConnection(fd, addr, addrLen);
}

void deleteSocket(socket_t *sock) {
	if (!sock) {
		return;
	}

	close(sock->fd);
	free(sock);
}