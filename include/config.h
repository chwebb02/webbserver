#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdbool.h>
#include <sys/types.h>

#define MAX_WEBSERVER_ROOT_DIR_PATH_LEN 512
#define MAX_WEBSERVER_INDEX_FILE_NAME_LEN 64

typedef struct _configInternalStruct _configInternal_t;

/// @brief Structure for storing and handing off configuration data read from a config file
typedef struct configStruct {
	_configInternal_t *_private;
	unsigned short webserverPort;
	bool findNextAvailablePort;
	bool ipv6;
	unsigned short connectionQueueMaxSize;
	unsigned short threadpoolSize;
	unsigned short threadpoolBusyWaitTimer;
	size_t requestMappingDomainSize;
	char webserverRootDir[MAX_WEBSERVER_ROOT_DIR_PATH_LEN];
	char webserverIndexFileName[MAX_WEBSERVER_INDEX_FILE_NAME_LEN];
} config_t;

#endif /* _CONFIG_H_ */