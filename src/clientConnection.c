#include "clientConnection.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "http.h"
#include <string.h>

struct clientConnectionStruct {
	int fd;
	struct sockaddr_in addr;
	socklen_t addrLen;
};

clientConnection_t *createClientConnection(int connectionFD, struct sockaddr_in addr, socklen_t addrLen) {
	if (connectionFD < 0) {
		return NULL;
	}

	clientConnection_t *out = malloc(sizeof(clientConnection_t));
	if (!out) {
		return NULL;
	}

	out->fd = connectionFD;

	// May need to make use of addr, so we store
	// even though no use as of this moment
	out->addr = addr;
	out->addrLen = addrLen;

	return out;
}

httpRequest_t *awaitClientRequest(clientConnection_t *cc) {
	if (!cc) {
		return NULL;
	}

	char buffer[BUFSIZ];
	recv(cc->fd, buffer, BUFSIZ * sizeof(char), 0);

	printf("\nRECEIVED REQUEST:\n%s", buffer);

	return deserializeHttpRequest(buffer);
}

int sendHttpResponse(clientConnection_t *cc, httpResponse_t *resp) {
	if (!cc || !resp) {
		return 2;
	}

	char *netResp = serializeHttpResponse(resp);
	if (!netResp) {
		return 1;
	}

	send(cc->fd, netResp, (strlen(netResp) + 1) * sizeof(char), 0);

	free(netResp);
	return 0;
}

void deleteClientConnection(clientConnection_t *cc) {
	if (!cc) {
		return;
	}

	close(cc->fd);
	free(cc);
}