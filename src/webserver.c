#include "webserver.h"
#include "socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "clientConnection.h"
#include "queue.h"
#include "threadpool.h"
#include <pthread.h>
#include "requestHandler.h"
#include <string.h>

struct webserverStruct {
	socket_t *socket;
	queue_t *connectionQueue;
	pthread_t connectionListener;
	threadpool_t *threadpool;
	unsigned short busyWaitTimer;
	unsigned short threadCount;
	bool stopConnectionListener;
	requestHandler_t *reqHandler;
	size_t requestMappingDomainSize;
	char *staticContentRootDir;
	char *staticContentIndexFileName;
};

webserver_t *registerWebserver(config_t *conf) {
	if (!conf) {
		return NULL;
	}

	webserver_t *out = malloc(sizeof(webserver_t));
	if (!out) {
		return NULL;
	}

	out->socket = createSocket(conf);
	if (!out->socket) {
		free(out);
		return NULL;
	}

	out->threadCount = conf->threadpoolSize;
	out->busyWaitTimer = conf->threadpoolBusyWaitTimer;
	out->stopConnectionListener = false;
	out->requestMappingDomainSize = conf->requestMappingDomainSize;

	out->staticContentRootDir = malloc(strlen(conf->webserverRootDir) * sizeof(char));
	if (!out->staticContentRootDir) {
		deleteSocket(out->socket);
		free(out);
		return NULL;
	}
	strncpy(out->staticContentRootDir, conf->webserverRootDir, strlen(conf->webserverRootDir));

	return out;
}

int processTransaction(webserver_t *ws) {
	if (!ws) {
		return 2;
	}

	clientConnection_t *connection = queueGet(ws->connectionQueue);
	if (!connection) {
		return 1;
	}

	// Receive request
	httpRequest_t *request = awaitClientRequest(connection);

	// Generate response
	httpResponse_t *response = respondToRequest(ws->reqHandler, request);

	// Send response
	if (sendHttpResponse(connection, response)) {
		printf("Failed to send HTTP response to client.\n");
	}

	// Close connection
	deleteClientConnection(connection);
}

void connectionListenerFunction(webserver_t *ws) {
	if (!ws) {
		return;
	}

	while (1) {
		// Accept connection
		clientConnection_t *connection = socketAcceptConnection(ws->socket);
		if (!connection) {
			perror("Failed to accept connection");
			continue;
		}

		queueAdd(ws->connectionQueue, connection);
		
		if (ws->stopConnectionListener) {
			break;
		}
	}
}

int startWebserver(webserver_t *ws) {
	if (!ws) {
		return 4;
	}

	// Create the queue for incoming connections
	ws->connectionQueue = createQueue((void (*)(void *)) deleteClientConnection);
	if (!ws->connectionQueue) {
		deleteSocket(ws->socket);
		free(ws);
		return 1;
	}

	// Create a request handler to map requests
	ws->reqHandler = createRequestHandler(ws->requestMappingDomainSize, ws->staticContentRootDir, ws->staticContentIndexFileName);
	if (!ws->reqHandler) {
		free(ws->connectionQueue);
		deleteSocket(ws->socket);
		free(ws);
		return 2;
	}

	// Create a threadpool that processes transactions as connections are added to the queue
	ws->threadpool = createThreadpool(ws->threadCount, ws->busyWaitTimer, (void *(*)(void *)) processTransaction, ws);
	if (!ws->threadpool) {
		deleteRequestHandler(ws->reqHandler);
		deleteQueue(ws->connectionQueue);
		deleteSocket(ws->socket);
		free(ws);
		return 3;
	}

	// Create a connection listener thread to add incoming connections to the queue
	pthread_create(&ws->connectionListener, NULL, (void *(*)(void *)) connectionListenerFunction, ws);

	printf("Started webserver on port %d.\n", socketGetBoundPort(ws->socket));
}

int webserverSendCommand(webserver_t *ws, webserverManagementCommand_t command) {
	if (!ws) {
		return 2;
	}

	switch (command) {
		case STOP:
		case RECONFIGURE:
		default:
			printf("Invalid command or command not yet implemented.\n");
			return 1;
	}

	return 0;
}

void deleteWebserver(webserver_t *ws) {
	if (!ws) {
		return;
	}

	unsigned short port = socketGetBoundPort(ws->socket);
	printf("Shutting down webserver on port %d...\n", port);

	pthread_cancel(ws->connectionListener);
	pthread_join(ws->connectionListener, NULL);
	deleteThreadpool(ws->threadpool);
	deleteQueue(ws->connectionQueue);
	deleteSocket(ws->socket);
	free(ws);

	printf("Shut down webserver on port %d.\n", port);
}