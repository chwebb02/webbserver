#include "hashmap.h"
#include <stdlib.h>
#include <string.h>

#ifndef node_t
typedef struct nodeStruct {
	void *item;
	char *unhashedKey;
	struct nodeStruct *next;
} node_t;
#endif /* node_t */

typedef struct hashmapListStruct {
	node_t *head;
	node_t *tail;
	size_t length;
} hashmapList_t;

struct hashmapStruct {
	hashmapList_t **array;			// Array of hashmapLists
	size_t arrayLength;
	hashFunction_t func;
	void (*freeItemFunc)(void *);
};

hashmapList_t *createHashmapList() {
	hashmapList_t *out = malloc(sizeof(hashmapList_t));
	if (!out) {
		return NULL;
	}

	out->head = NULL;
	out->tail = NULL;
	out->length = 0;

	return out;
}

node_t *createHashmapNode(void *item, const char *unhashedKey) {
	node_t *out = malloc(sizeof(node_t));
	if (!out) {
		return NULL;
	}

	out->item = item;
	out->unhashedKey = malloc((strlen(unhashedKey) + 1) * sizeof(char));
	if (!out->unhashedKey) {
		free(out);
		return NULL;
	}
	strcpy(out->unhashedKey, unhashedKey);
	out->next = NULL;

	return out;
}

int hashmapListInsert(hashmapList_t *list, void *item, const char *unhashedKey) {
	if (!list) {
		return 2;
	}

	node_t *insertion = createHashmapNode(item, unhashedKey);
	if (!insertion) {
		return 1;
	}

	if (!list->head) {
		list->head = insertion;
		list->tail = insertion;
	} else {
		list->tail->next = insertion;
	}

	list->tail = insertion;
	list->length += 1;

	return 0;
}

void *hashmapListSearch(hashmapList_t *list, const char *key) {
	if (!list || !key) {
		return NULL;
	}

	node_t *node = list->head;
	for (size_t i = 0; i < list->length; i++) {
		if (!strcmp(key, node->unhashedKey)) {
			return node->item;
		}

		node = node->next;
	}

	return NULL;
}

void deleteHashmapList(hashmapList_t *hml, void (*freeItemFunc)(void *)) {
	if (!hml) {
		return;
	}

	node_t *current = hml->head;
	node_t *next;
	for (size_t i = 0; i < hml->length; i++) {
		next = current->next;

		if (freeItemFunc) {
			freeItemFunc(current->item);
		}

		free(current->unhashedKey);
		free(current);

		current = next;
	}
	
	free(hml);
}

void *hashmapLookup(hashmap_t *map, const char *key) {
	if (!map || !key) {
		return NULL;
	}

	size_t index = map->func(map, key);
	if (index >= map->arrayLength) {
		return NULL;
	}

	return hashmapListSearch(map->array[index], key);
}

size_t defaultHashFunction(hashmap_t *map, const char *input) {
	if (!map || !input) {
		return 0;
	}

	int inputLength = strlen(input);
	size_t out = 0;
	for (int i = 1; i < inputLength - 1; i++) {
		out += ((size_t) input[i - 1]) * 3 + ((size_t) input[i]) + ((size_t) input[i + 1]) * 2;
		out %= map->arrayLength;
	}

	return out;
}

hashmap_t *createHashmap(size_t domainSize, hashFunction_t hashFunc, void (*freeItemFunc)(void *)) {
	if (domainSize < 1) {
		return NULL;
	}

	hashmap_t *out = malloc(sizeof(hashmap_t));
	if (!out) {
		return NULL;
	}

	out->freeItemFunc = freeItemFunc;

	if (!hashFunc) {
		out->func = defaultHashFunction;
	} else {
		out->func = hashFunc;
	}

	out->array = malloc(domainSize * sizeof(hashmapList_t *));
	if (!out->array) {
		free(out);
		return NULL;
	}

	for (out->arrayLength = 0; out->arrayLength < domainSize; out->arrayLength++) {
		out->array[out->arrayLength] = createHashmapList();
		if (!out->array[out->arrayLength]) {
			for (out->arrayLength -= 1; out->arrayLength >= 0; out->arrayLength--) {
				deleteHashmapList(out->array[out->arrayLength], freeItemFunc);
				return NULL;
			}
		}
	}

	return out;
}

int hashmapInsert(hashmap_t *map, const char *key, void *value) {
	if (!map || !key) {
		return 2;
	}

	size_t index = map->func(map, key);
	if (index >= map->arrayLength) {
		return 1;
	}

	return hashmapListInsert(map->array[index], value, key);
}

void deleteHashmap(hashmap_t *map) {
	if (!map) {
		return;
	}

	for (size_t i = 0; i < map->arrayLength; i++) {
		deleteHashmapList(map->array[i], map->freeItemFunc);
	}

	free(map->array);
	free(map);
}