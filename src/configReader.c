#include "configReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "hashmap.h"
#include "configProperties.h"

struct _configInternalStruct {
	hashmap_t *propertyMap;
};

typedef enum actionFuncReturnValueEnum {
	SUCCESS = 0,
	ZERO_HASH,
	INVALID_PROPERTY,
	INVALID_VALUE,
	CRITICAL
} actionReturn_t;

typedef actionReturn_t (*actionFunc_t)(config_t *, const char *);

int breakUp(const char *line, char *property, char *value, char delim) {
	for (size_t i = 0; i < strlen(line); i++) {
		if (line[i] == delim) {
			memcpy(property, line, i * sizeof(char));
			memcpy(value, line + i + 1, (strlen(line) - (i + 1)) * sizeof(char));

			if (value[strlen(value) - 1] == '\n' || value[strlen(value) - 1] == '\r') {
				value[strlen(value) - 1] = '\0';
			}

			return 0;
		}
	}

	return 1;
}

actionReturn_t setWebserverPort(config_t *conf, const char *value) {
	conf->webserverPort = atoi(value);
	return SUCCESS;
}

actionReturn_t setFindNextAvailablePort(config_t *conf, const char *value) {
	conf->findNextAvailablePort = !strcmp(value, "false") ? false : true;

	if (strcmp(value, "false") && strcmp(value, "true")) {
		printf("Invalid value '%s' for property '%s', defaulting to false\n", value, PROPERTY_FIND_NEXT_AVAILABLE_PORT_STRING);
		return INVALID_VALUE;
	}

	return SUCCESS;
}

actionReturn_t setUseIPv6(config_t *conf, const char *value) {
	printf("IPv6 not supported yet, but will be eventually\n");
	return SUCCESS;
}

actionReturn_t setWebserverConnectionQueueSize(config_t *conf, const char *value) {
	conf->connectionQueueMaxSize = atoi(value);
	return SUCCESS;
}

actionReturn_t setThreadpoolSize(config_t *conf, const char *value) {
	conf->threadpoolSize = atoi(value);
	return SUCCESS;
}

actionReturn_t setThreadpoolBusyWaitTimer(config_t *conf, const char *value) {
	conf->threadpoolBusyWaitTimer = atoi(value);
	return SUCCESS;
}

actionReturn_t setRequestMappingDomain(config_t *conf, const char *value) {
	conf->requestMappingDomainSize = atoi(value);
	return SUCCESS;
}

actionReturn_t setWebserverRootDir(config_t *conf, const char *value) {
	strncpy(conf->webserverRootDir, value, MAX_WEBSERVER_ROOT_DIR_PATH_LEN);
	return SUCCESS;
}

actionReturn_t setWebserverIndex(config_t *conf, const char *value) {
	strncpy(conf->webserverIndexFileName, value, MAX_WEBSERVER_INDEX_FILE_NAME_LEN);
	return SUCCESS;
}

actionReturn_t readLine(config_t *conf, FILE *fp) {
	// Grab a buffer to read configuration lines into
	char buffer[BUFSIZ];
	fgets(buffer, BUFSIZ * sizeof(char), fp);

	// Break buffer into key-value pairs
	char property[BUFSIZ];
	char value[BUFSIZ];
	memset(property, '\0', BUFSIZ * sizeof(char));
	memset(value, '\0', BUFSIZ * sizeof(char));
	breakUp(buffer, property, value, '=');

	if (strlen(property) < 1) {
		return ZERO_HASH;
	}

	actionFunc_t action = hashmapLookup(conf->_private->propertyMap, property);
	if (action == NULL) {
		printf("Unknown configuration property: %s\n", property);
		return INVALID_PROPERTY;
	}

	return action(conf, value);
}

void setDefaults(config_t *conf) {
	conf->webserverPort = CONFIG_DEFAULT_WEBSERVER_PORT;
	conf->findNextAvailablePort = CONFIG_DEFAULT_FIND_NEXT_AVAILABLE_PORT;
	conf->ipv6 = CONFIG_DEFAULT_IPV6;
	conf->connectionQueueMaxSize = CONFIG_DEFAULT_CONNECTION_QUEUE_MAX_SIZE;
	conf->threadpoolSize = CONFIG_DEFAULT_THREADPOOL_SIZE;
	conf->threadpoolBusyWaitTimer = CONFIG_DEFAULT_THREADPOOL_BUSY_WAIT_TIMER;
	conf->requestMappingDomainSize = CONFIG_DEFAULT_REQUEST_MAPPING_DOMAIN_SIZE;

	strncpy(conf->webserverRootDir, CONFIG_DEFAULT_WEBSERVER_ROOT_DIR, (strlen(CONFIG_DEFAULT_WEBSERVER_ROOT_DIR) + 1) * sizeof(char));
	strncpy(conf->webserverIndexFileName, CONFIG_DEFAULT_WEBSERVER_INDEX, (strlen(CONFIG_DEFAULT_WEBSERVER_INDEX) + 1) * sizeof(char));
}

int populatePropertyMap(config_t *conf) {
	int status = 0;
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_WEBSERVER_PORT_STRING, (actionFunc_t) setWebserverPort);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_FIND_NEXT_AVAILABLE_PORT_STRING, (actionFunc_t) setFindNextAvailablePort);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_WEBSERVER_CONNECTION_QUEUE_SIZE_STRING, (actionFunc_t) setWebserverConnectionQueueSize);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_WEBSERVER_USE_IPV6_STRING, (actionFunc_t) setUseIPv6);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_THREADPOOL_SIZE_STRING, (actionFunc_t) setThreadpoolSize);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_THREADPOOL_BUSY_WAIT_TIMER_STRING, (actionFunc_t) setThreadpoolBusyWaitTimer);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_REQUEST_MAPPING_DOMAIN_SIZE_STRING, (actionFunc_t) setRequestMappingDomain);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_WEBSERVER_ROOT_DIR_STRING, (actionFunc_t) setWebserverRootDir);
	status += hashmapInsert(conf->_private->propertyMap, PROPERTY_WEBSERVER_INDEX_STRING, (actionFunc_t) setWebserverIndex);

	return status;
}

config_t *readConfigurationFile(const char *propertiesFile) {
	// Likely redundant because of fopen() call, but overhead is negligible
	if (!propertiesFile) {
		return NULL;
	}

	config_t *out = malloc(sizeof(config_t));
	if (!out) {
		return NULL;
	}

	out->_private = malloc(sizeof(_configInternal_t));
	if (!out->_private) {
		free(out);
		return NULL;
	}

	out->_private->propertyMap = createHashmap(TOTAL_PROPERTY_COUNT * 1.5, NULL);
	if (!out->_private->propertyMap) {
		free(out->_private);
		free(out);
		return NULL;
	}

	// Setup
	setDefaults(out);
	if (populatePropertyMap(out)) {
		printf("Failed to map properties\n");
		deleteConfig(out);
		return NULL;
	}

	FILE *fp = fopen(propertiesFile, "r");
	if (!fp) {
		free(out);
		return NULL;
	}

	// Read configuration optiosn into config_t
	while (!feof(fp)) {
		int status = readLine(out, fp);

		// May need to handle situations where status is a critical error here
		if (status == CRITICAL) {
			fclose(fp);
			deleteConfig(out);
			return NULL;
		}
	}

	fclose(fp);
	return out;
}

void deleteConfig(config_t *conf) {
	deleteHashmap(conf->_private->propertyMap);
	free(conf->_private);
	free(conf);
}