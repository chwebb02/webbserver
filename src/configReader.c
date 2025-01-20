#include "configReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

// POTENTIAL TODO: Replace configuration property hash with library hashmap

#define HASH_MODULO 100000000

enum readLineErrors {
	SUCCESS = 0,
	ZERO_HASH,
	INVALID_PROPERTY,
	INVALID_VALUE
};

int breakUp(const char *line, char *property, char *value, char delim) {
	for (int i = 0; i < strlen(line); i++) {
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

ssize_t hashProperty(const char *prop) {
	ssize_t out = 0;
	for (int i = 0; i < strlen(prop); i++) {
		out *= 10;
		out += (ssize_t) prop[i];
		out %= HASH_MODULO;
	}
	
	return out;
}

ssize_t WEBSERVER_PORT_HASH;
ssize_t FIND_NEXT_AVAILABLE_PORT_HASH;
ssize_t WEBSERVER_USE_IPV6_HASH;
ssize_t WEBSERVER_CONNECTION_QUEUE_SIZE_HASH;
ssize_t THREADPOOL_SIZE_HASH;
ssize_t THREADPOOL_BUSY_WAIT_TIMER_HASH;
ssize_t REQUEST_MAPPING_DOMAIN_HASH;
ssize_t WEBSERVER_ROOT_DIR_HASH;
ssize_t WEBSERVER_INDEX_HASH;

int readLine(config_t *conf, FILE *fp) {
	// Grab a buffer to read configuration lines into
	char buffer[BUFSIZ];
	fgets(buffer, BUFSIZ * sizeof(char), fp);

	// Break buffer into key-value pairs
	char property[BUFSIZ];
	char value[BUFSIZ];
	memset(property, '\0', BUFSIZ * sizeof(char));
	memset(value, '\0', BUFSIZ * sizeof(char));
	breakUp(buffer, property, value, '=');

	ssize_t hash = hashProperty(property);

	// TODO: Implement proper logic for value checking with atoi and other such functions

	if (hash == 0) {
		return ZERO_HASH;
	} else if (hash == WEBSERVER_PORT_HASH) {
		conf->webserverPort = atoi(value);
	} else if (hash == FIND_NEXT_AVAILABLE_PORT_HASH) {
		conf->findNextAvailablePort = !strcmp(value, "false") ? false : true;

		if (strcmp(value, "false") && strcmp(value, "true")) {
			printf("Invalid value '%s' for property '%s', defaulting to false\n", value, property);
			return INVALID_VALUE;
		}
	} else if (hash == WEBSERVER_USE_IPV6_HASH) {
		printf("IPv6 not supported yet, but will be eventually\n");
	} else if (hash == WEBSERVER_CONNECTION_QUEUE_SIZE_HASH) {
		conf->connectionQueueMaxSize = atoi(value);
	} else if (hash == THREADPOOL_SIZE_HASH) {
		conf->threadpoolSize = atoi(value);
	} else if (hash == THREADPOOL_BUSY_WAIT_TIMER_HASH) {
		conf->threadpoolBusyWaitTimer = atoi(value);
	} else if (hash == REQUEST_MAPPING_DOMAIN_HASH) {
		conf->requestMappingDomainSize = atoi(value);	
	} else if (hash == WEBSERVER_ROOT_DIR_HASH) {
		strncpy(conf->webserverRootDir, value, MAX_WEBSERVER_ROOT_DIR_PATH_LEN);
	} else if (hash == WEBSERVER_INDEX_HASH) {
		strncpy(conf->webserverIndexFileName, value, MAX_WEBSERVER_INDEX_FILE_NAME_LEN);
	} else {
		printf("Unknown configuration property: '%s'\n", property);
		return INVALID_PROPERTY;
	}

	return SUCCESS;
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

// Precompute hashes for optimization
ssize_t hashValues[TOTAL_PROPERTY_COUNT];
void precomputeHashes() {
	hashValues[0] = WEBSERVER_PORT_HASH = hashProperty("webserver-port");
	hashValues[1] = FIND_NEXT_AVAILABLE_PORT_HASH = hashProperty("find-next-available-port");
	hashValues[2] = WEBSERVER_USE_IPV6_HASH = hashProperty("webserver-use-ipv6");
	hashValues[3] = WEBSERVER_CONNECTION_QUEUE_SIZE_HASH = hashProperty("webserver-connection-queue-size");
	hashValues[4] = THREADPOOL_SIZE_HASH = hashProperty("threadpool-size");
	hashValues[5] = THREADPOOL_BUSY_WAIT_TIMER_HASH = hashProperty("threadpool-busy-wait-timer");
	hashValues[6] = REQUEST_MAPPING_DOMAIN_HASH = hashProperty("request-mapping-domain");
	hashValues[7] = WEBSERVER_ROOT_DIR_HASH = hashProperty("webserver-root-dir");
	hashValues[8] = WEBSERVER_INDEX_HASH = hashProperty("webserver-index");
}

bool verifyHashIntegrity() {
	for (int i = 0; i < TOTAL_PROPERTY_COUNT; i++) {
		for (int j = 0; j < TOTAL_PROPERTY_COUNT; j++) {
			if (j == i) {
				continue;
			}

			if (hashValues[i] == hashValues[j]) {
				return false;
			}
		}
	}

	return true;
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

	// Setup
	setDefaults(out);
	precomputeHashes();
	if (!verifyHashIntegrity()) {
		printf("Configuration hash integrity check failed!\n");
		exit(1);
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
	}

	fclose(fp);
	return out;
}

void deleteConfig(config_t *conf) {
	free(conf);
}