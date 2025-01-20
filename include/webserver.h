#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "config.h"

/// @brief A structure defining the main webserver object 
typedef struct webserverStruct webserver_t;

/// @brief The list of commands that can be supplied to the webserver
typedef enum webserverManagementCommandEnum {
	STOP,
	RECONFIGURE
} webserverManagementCommand_t;

/// @brief Register a web server, reserving space for it and setting the configuration options
/// @param conf The config object
/// @return The webserver object, or NULL on error
/// @note After registering, a webserver must be started before it is useable
webserver_t *registerWebserver(config_t *conf);

/// @brief Start a webserver that has been registered already
/// @param ws The webserver object
/// @return 0 on success, or an error code on failure
int startWebserver(webserver_t *ws);

/// @brief Manage the running webserver by sending a command
/// @param ws The webserver object
/// @param command The command to execute
/// @return 0 on success, or an error code on failure
int webserverSendCommand(webserver_t *ws, webserverManagementCommand_t command);

/// @brief Get the port that a webserver is bound to
/// @param ws The webserver object
/// @return The port number on success, or 0 on failure
unsigned short getWebserverPort(webserver_t *ws);

/// @brief Release all memory associated with the webserver
/// @param ws The webserver object
void deleteWebserver(webserver_t *ws);

#endif /* _WEBSERVER_H_ */