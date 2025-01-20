#include "staticContentServer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

struct staticContentServerStruct {
	char *rootDir;
	char *indexFileName;
};

staticContentServer_t *createStaticContentServer(const char *staticContentRootDir, const char *indexFileName) {
	if (!staticContentRootDir) {
		return NULL;
	}

	staticContentServer_t *out = malloc(sizeof(staticContentServer_t));
	if (!out) {
		return NULL;
	}

	out->rootDir = malloc((strlen(staticContentRootDir) + 1) * sizeof(char));
	if (!out->rootDir) {
		free(out);
		return NULL;
	}

	out->indexFileName = malloc((strlen(indexFileName) + 2) * sizeof(char));
	if (!out->indexFileName) {
		free(out->rootDir);
		free(out);
		return NULL;
	}

	strncpy(out->rootDir, staticContentRootDir, strlen(staticContentRootDir));
	
	out->indexFileName[0] = "/";
	strcat(out->indexFileName, indexFileName);

	// Ensure that there is no slash at the end of the rootDir
	while (out->rootDir[strlen(out->rootDir) - 1] == '/') {
		out->rootDir[strlen(out->rootDir) - 1] = '\0';
	}

	return out;
}

httpResponse_t *generateStaticHttpResponse(const char *filepath) {
	if (!filepath) {
		return NULL;
	}

	httpResponse_t *out = malloc(sizeof(httpResponse_t));
	if (!out) {
		return NULL;
	}
	
	FILE *fp = fopen(filepath, "r");
	if (!fp) {
		free(out);
		return NULL;
	}

	return out;
}

httpResponse_t *serveStaticContent(staticContentServer_t *scs, const char *staticContentUri) {
	if (!scs || !staticContentUri) {
		return NULL;
	}

	char *filepath = malloc((strlen(scs->rootDir) + strlen(staticContentUri) + 1) * sizeof(char));
	if (!filepath) {
		return NULL;
	}

	strcpy(filepath, scs->rootDir);

	// Handle special case for index
	if (!strncmp(staticContentUri, "/", 1)) {
		strcat(filepath, scs->indexFileName);
	} else {
		strcat(filepath, staticContentUri);
	}
	
	httpResponse_t *out = generateStaticHttpResponse(filepath);
	free(filepath);

	return out;
}

void deleteStaticContentServer(staticContentServer_t *scs) {
	if (!scs) {
		return;
	}

	free(scs);
}