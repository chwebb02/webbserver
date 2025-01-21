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
#include "terminal.h"

struct webserverStruct {
	socket_t *socket;
	queue_t *connectionQueue;
	pthread_t connectionListener;
	threadpool_t *threadpool;
	unsigned short busyWaitTimer;
	unsigned short threadCount;
	requestHandler_t *reqHandler;
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

	out->reqHandler = createRequestHandler(conf);
	if (!out->reqHandler) {
		deleteSocket(out->socket);
		free(out);
		return NULL;
	}

	// Create the queue for incoming connections
	out->connectionQueue = createQueue((void (*)(void *)) deleteClientConnection);
	if (!out->connectionQueue) {
		deleteRequestHandler(out->reqHandler);
		deleteSocket(out->socket);
		free(out);
		return NULL;
	}

	out->threadCount = conf->threadpoolSize;
	out->busyWaitTimer = conf->threadpoolBusyWaitTimer;

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
		terminalSendMessage("Failed to send HTTP response to client.\n");
	}

	// Close connection
	deleteClientConnection(connection);
	return 0;
}

void connectionListenerFunction(webserver_t *ws) {
	if (!ws) {
		return;
	}

	while (1) {
		// Accept connection
		clientConnection_t *connection = socketAcceptConnection(ws->socket);
		if (!connection) {
			// printf("Failed to accept connection");
			continue;
		}
		
		if (queueAdd(ws->connectionQueue, connection)) {
			// printf("Failed to add connection to queue.\n");
			deleteClientConnection(connection);
			continue;
		}
	}
}

int startWebserver(webserver_t *ws) {
	if (!ws) {
		return 4;
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

	return 0;
}

unsigned short getWebserverPort(webserver_t *ws) {
	if (!ws) {
		return 0;
	}

	return socketGetBoundPort(ws->socket);
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

	pthread_cancel(ws->connectionListener);
	pthread_join(ws->connectionListener, NULL);
	deleteThreadpool(ws->threadpool);
	deleteQueue(ws->connectionQueue);
	deleteRequestHandler(ws->reqHandler);
	deleteSocket(ws->socket);
	free(ws);
}