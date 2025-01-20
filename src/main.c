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
	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}

	// Set terminal to non-blocking
	// prevents one thread from blocking input from other threads
	setvbuf(stdout, NULL, _IONBF, 0);

	// Begin configuration step
	printf("\nReading configuration...\n");
	config_t *conf = readConfigurationFile(argv[1]);
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
		deleteConfig(conf);
		exit(1);
	}
	printf("Webserver registered.\n");

	// Initialize the terminal for concurrent messages
	if (initTerminal(conf)) {
		printf("Could not change terminal modes.\n");
		deleteWebserver(webserver);
		deleteConfig(conf);
		exit(1);
	}

	deleteConfig(conf);
	// End configuration step

	printf("\nStarting webserver on port...\n");
	if (startWebserver(webserver)) {		// Main program logic is handled here
		printf("Could not start the webserver.\n");
		deleteTerminal();
		deleteWebserver(webserver);
		exit(1);
	}
	printf("Webserver started on port %d\n", getWebserverPort(webserver));

	terminalSendMessage("Webserver time!");
	char buffer[BUFSIZ];
	while (1) {
		fgets(buffer, BUFSIZ * sizeof(char), stdin);
		terminalSendMessage("Unknown command!");
		sleep(2);
	}
	
	deleteTerminal();
	deleteWebserver(webserver);
	// Shutdown the webserver

	return 0;
}