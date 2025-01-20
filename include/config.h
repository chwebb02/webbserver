#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdbool.h>
#include <sys/types.h>

#define MAX_WEBSERVER_ROOT_DIR_PATH_LEN 512
#define MAX_WEBSERVER_INDEX_FILE_NAME_LEN 64
#define TOTAL_PROPERTY_COUNT 9

#define CONFIG_DEFAULT_WEBSERVER_PORT 8019
#define CONFIG_DEFAULT_FIND_NEXT_AVAILABLE_PORT false
#define CONFIG_DEFAULT_IPV6 false
#define CONFIG_DEFAULT_CONNECTION_QUEUE_MAX_SIZE 25
#define CONFIG_DEFAULT_THREADPOOL_SIZE 8
#define CONFIG_DEFAULT_THREADPOOL_BUSY_WAIT_TIMER 1
#define CONFIG_DEFAULT_REQUEST_MAPPING_DOMAIN_SIZE 100
#define CONFIG_DEFAULT_WEBSERVER_ROOT_DIR "../resources/static"
#define CONFIG_DEFAULT_WEBSERVER_INDEX "index.html"

/// @brief Structure for storing and handing off configuration data read from a config file
typedef struct configStruct {
	unsigned short webserverPort;
	bool findNextAvailablePort;
	bool ipv6;
	unsigned short connectionQueueMaxSize;
	unsigned short threadpoolSize;
	unsigned short threadpoolBusyWaitTimer;
	size_t requestMappingDomainSize;
	char webserverRootDir[MAX_WEBSERVER_ROOT_DIR_PATH_LEN];
	char webserverIndexFileName[MAX_WEBSERVER_INDEX_FILE_NAME_LEN]
} config_t;

#endif /* _CONFIG_H_ */