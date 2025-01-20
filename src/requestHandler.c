#include "requestHandler.h"
#include <stdlib.h>
#include "hashmap.h"
#include "staticContentServer.h"
#include <string.h>

struct requestHandlerStruct {
	hashmap_t *uriMappings;
	staticContentServer_t *staticServer;
};

#include <stdio.h>

requestHandler_t *createRequestHandler(config_t *conf) {
	if (conf->requestMappingDomainSize < 1 || !conf->webserverRootDir) {
		return NULL;
	}

	requestHandler_t *out = malloc(sizeof(requestHandler_t));
	if (!out) {
		return NULL;
	}

	out->uriMappings = createHashmap(conf->requestMappingDomainSize, NULL);
	if (!out->uriMappings) {
		free(out);
		return NULL;
	}

	out->staticServer = createStaticContentServer(conf);
	if (!out->staticServer) {
		deleteHashmap(out->uriMappings);
		free(out);
		return NULL;
	}

	return out;
}

int registerEndpoint(requestHandler_t *reqHandler, const char *uriPath, handlerFunction action) {
	if (!reqHandler || !uriPath || !action) {
		return 3;
	}
	
	if (strlen(uriPath) < 1 || action == NULL || hashmapLookup(reqHandler->uriMappings, uriPath)) {
		return 1;
	}

	if (hashmapLookup(reqHandler->uriMappings, uriPath)) {
		return 4;
	}

	if (hashmapInsert(reqHandler->uriMappings, uriPath, action)) {
		return 2;
	}

	return 0;
}

// POTENTIAL TODO: Consider implementing deregistering and altering registration of endpoints

httpResponse_t *respondToRequest(requestHandler_t *reqHandler, httpRequest_t *req) {
	if (!reqHandler || !req) {
		return NULL;
	}

	handlerFunction action = hashmapLookup(reqHandler->uriMappings, req->header.uri);
	if (!action) {
		httpResponse_t *staticResponse = serveStaticContent(reqHandler->staticServer, req->header.uri);
		if (!staticResponse) {
			return NULL;
		}

		return staticResponse;
	}

	// There may be a way to do this that is more user friendly
	return action(req);
}

void deleteRequestHandler(requestHandler_t *reqHandler) {
	if (!reqHandler) {
		return;
	}

	deleteStaticContentServer(reqHandler->staticServer);
	deleteHashmap(reqHandler->uriMappings);
	free(reqHandler);
}