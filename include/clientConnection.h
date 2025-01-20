#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include <netinet/in.h>
#include "http.h"

/// @brief Object for representing connections from client machines to the server
typedef struct clientConnectionStruct clientConnection_t;

/// @brief Create a client connection object
/// @param connectionFD The file descriptor of the connection
/// @param addr The address of the connection
/// @param addrLen The size of the addr variable
/// @return A new client connection object or NULL on error
clientConnection_t *createClientConnection(int connectionFD, struct sockaddr_in addr, socklen_t addrLen);

/// @brief Wait for and receive a request from a client connection
/// @param cc The client connection object
/// @return The HTTP request from the client connection, or NULL on error
httpRequest_t *awaitClientRequest(clientConnection_t *cc);

/// @brief Send an HTTP response to the client connection
/// @param cc The client connection object
/// @param resp The HTTP response object
/// @return 0 on success, or an error code on failure
int sendHttpResponse(clientConnection_t *cc, httpResponse_t *resp);

/// @brief Release memory associated with client connection
/// @param cc The client connection object
void deleteClientConnection(clientConnection_t *cc);

#endif /* _CLIENT_CONNECTION_H_ */