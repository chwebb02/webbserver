#ifndef _STATIC_CONTENT_SERVER_H_
#define _STATIC_CONTENT_SERVER_H_

#include "http.h"

/// @brief A structure that assists the request handler in serving static content
typedef struct staticContentServerStruct staticContentServer_t;

/// @brief Create a new static content server
/// @param staticContentRootDir The root directory for finding static content
/// @return The static content server object, or NULL on error
staticContentServer_t *createStaticContentServer(const char *staticContentRootDir);

/// @brief Respond to an HTTP request for static content
/// @param scs The static content server object
/// @param staticContentUri The relative path from the static content root directory to the static file
/// @return An HTTP response to the request, or NULL on error
httpResponse_t *serveStaticContent(staticContentServer_t *scs, const char *staticContentUri);

/// @brief Release all memory associated with the static content server
/// @param scs The static content server object
void deleteStaticContentServer(staticContentServer_t *scs);

#endif /* _STATIC_CONTENT_SERVER_H_ */