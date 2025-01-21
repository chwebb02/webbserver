#include <stdio.h>
#include <stdlib.h>
#include "configReader.h"
#include "webserver.h"
#include <unistd.h>
#include "terminal.h"
#include <string.h>

void usage(const char *execName) {
	printf("\n\tUSAGE: %s <configuration file>\n\n", execName);
}

int main(int argc, char **argv) {
	char *configFile = NULL;
	if (argc > 1 && !strncmp(argv[1], "-h", 2)) {
		usage(argv[0]);
		exit(1);
	}

	printf("\nSearching for config file...\n");
	if (argc > 1) {
		printf("Config file supplied via command line.\n");
		configFile = argv[1];
	} else {
		printf("No config file specified, searching...\n");
		configFile = findConfigFile();
		if (configFile == NULL) {
			printf("Could not automatically locate a config file, please provide one.\n");
			usage(argv[0]);
			exit(1);
		}
	}
	printf("Found config file at %s.\n", configFile);

	// Set terminal to non-blocking
	// prevents one thread from blocking input from other threads
	setvbuf(stdout, NULL, _IONBF, 0);

	// Begin configuration step
	printf("\nReading configuration...\n");
	config_t *conf = readConfigurationFile(configFile);
	if (!conf) {
		perror("Could not read configuration file");
		exit(1);
	}
	printf("Configuration file read.\n");

	// Register the webserver
	printf("\nRegistering webserver...\n");
	webserver_t *webserver = registerWebserver(conf);
	if (!webserver) {
		perror("Could not register the webserver");
		
		if (argc < 2) {
			free(configFile);
		}	
		
		deleteConfig(conf);
		exit(1);
	}
	if (argc < 2) {
		free(configFile);
	}
	printf("Webserver registered.\n");

	// Initialize the terminal for concurrent messages
	printf("\nInitializing terminal...\n");
	if (initTerminal(conf)) {
		printf("Could not change terminal modes.\n");
		deleteWebserver(webserver);
		deleteConfig(conf);
		exit(1);
	}
	printf("Terminal initialized.\n");

	deleteConfig(conf);
	// End configuration step

	printf("\nStarting webserver on port...\n");
	if (startWebserver(webserver)) {		// Main program logic is handled here
		printf("Could not start the webserver.\n");
		deleteTerminal();
		deleteWebserver(webserver);
		exit(1);
	}
	unsigned short boundPort = getWebserverPort(webserver);
	printf("Webserver started on port %d.\n", boundPort);

	terminalSendMessage("SERVER CONSOLE:");
	char buffer[BUFSIZ];
	while (1) {
		fgets(buffer, BUFSIZ * sizeof(char), stdin);
		buffer[strlen(buffer) - 1] = '\0';

		if (!strncmp(buffer, "stop", 4)) {
			break;
		} else {
			terminalSendMessage("Unknown command!");
		}
	}
	
	printf("\nShutting down webserver on port %d...\n", boundPort);
	deleteTerminal();
	deleteWebserver(webserver);
	printf("Shut down webserver on port %d.\n", boundPort);
	// Shutdown the webserver

	printf("\n");
	return 0;
}