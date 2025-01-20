#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "http.h"
#include <sys/types.h>
#include "config.h"

/// @brief A structure for managing request handling on the server
typedef struct requestHandlerStruct requestHandler_t;

/// @brief Function type for custom handling logic of non-static content
typedef httpResponse_t *(*handlerFunction)(void *);

/// @brief Create a new request handler object
/// @param uriMappingDomainSize The size of the URI Mapping Domain
/// @param staticContentRootDir The root directory for static content served by the server
/// @param staticContentIndexFileName The name of the index file for static content relative to the static content root directory
/// @return A new request handler object, or NULL on error
requestHandler_t *createRequestHandler(config_t *conf);

/// @brief Register a new API endpoint on the server
/// @param reqHandler The request handler object
/// @param uriPath The API endpoint
/// @param action The function describing the logic that is triggered when the endpoint is visisted
/// @return 0 on success, or an error code on failure
int registerEndpoint(requestHandler_t* reqHandler, const char *uriPath, handlerFunction action);

/// @brief Apply the mapping rules to an incoming request and generate an HTTP response
/// @param reqHandler The request handler object
/// @param req The request object
/// @return An HTTP response corresponding to the request, or NULL on error
httpResponse_t *respondToRequest(requestHandler_t *reqHandler, httpRequest_t *req);

/// @brief Release all memory associated with the request handler
/// @param reqHandler The request handler object
void deleteRequestHandler(requestHandler_t *reqHandler);

#endif /* _REQUEST_HANDLER_H_ */