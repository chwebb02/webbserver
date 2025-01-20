#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "config.h"
#include "clientConnection.h"

/// @brief A structure representing a low-level socket for communication
typedef struct socketStruct socket_t;

/// @brief Create a new socket object based on the server configuration
/// @param conf The config option
/// @return A new socket object, or NULL on error
socket_t *createSocket(config_t *conf);

/// @brief Get the port that a socket was bound to
/// @param sock The socket object
/// @return The port that the socket is bound to, or 0 if it is not bound or there is an error
unsigned short socketGetBoundPort(socket_t *sock);

/// @brief Wait for and accept a connection on a socket
/// @param sock The socket object
/// @return A connection object, or NULL on error
clientConnection_t *socketAcceptConnection(socket_t *sock);

/// @brief Release all memory associated with the socket
/// @param sock The socket object
void deleteSocket(socket_t *sock);

#endif /* _WEBSOCKET_H_ */