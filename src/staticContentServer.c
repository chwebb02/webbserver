#include "staticContentServer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct staticContentServerStruct {
	char *rootDir;
};

staticContentServer_t *createStaticContentServer(const char *staticContentRootDir) {
	if (!staticContentRootDir) {
		return NULL;
	}

	staticContentServer_t *out = malloc(sizeof(staticContentServer_t));
	if (!out) {
		return NULL;
	}

	out->rootDir = malloc(strlen(staticContentRootDir) * sizeof(char));
	if (!out->rootDir) {
		free(out);
		return NULL;
	}

	strncpy(out->rootDir, staticContentRootDir, strlen(staticContentRootDir));

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
	strcat(filepath, staticContentUri);
	
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