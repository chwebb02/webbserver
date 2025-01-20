#include <stdio.h>
#include <stdlib.h>
#include "configReader.h"
#include "webserver.h"
#include <unistd.h>

void usage(const char *execName) {
	printf("\n\tUSAGE: %s <configuration file>\n\n", execName);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}

	// Set terminal to non-blocking
	// prevents one thread from blocking input from other threads
	setvbuf(stdout, NULL, _IONBF, 0);

	// Begin configuration step
	printf("Reading configuration...\n");
	config_t *conf = readConfigurationFile(argv[1]);
	if (!conf) {
		perror("Could not read configuration file");
		exit(1);
	}
	printf("Configuration file read.\n");

	// Register the webserver
	webserver_t *webserver = registerWebserver(conf);
	if (!webserver) {
		perror("Could not register the webserver");
		deleteConfig(conf);
		exit(1);
	}

	deleteConfig(conf);
	// End configuration step

	startWebserver(webserver);		// Main program logic is handled here

	while (1) {
		// printf(".");
		sleep(2);
	}
	
	deleteWebserver(webserver);
	// Shutdown the webserver

	return 0;
}