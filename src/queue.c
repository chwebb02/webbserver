#include "queue.h"
#include <pthread.h>
#include <stdlib.h>

#ifndef node_t
typedef struct nodeStruct {
	void *item;
	struct nodeStruct *next;	
} node_t;
#endif /* node_t */

struct queueStruct {
	node_t *head;
	node_t *tail;
	size_t length;
	void (*freeMethod)(void *);
	pthread_mutex_t mutex;
};

queue_t *createQueue(void (*freeMethod)(void *)) {
	queue_t *out = malloc(sizeof(queue_t));
	if (!out) {
		return NULL;
	}

	out->head = NULL;
	out->tail = NULL;
	out->length = 0;
	out->freeMethod = freeMethod;
	pthread_mutex_init(&out->mutex, NULL);

	return out;
}

node_t *createNode(void *item) {
	node_t *out = malloc(sizeof(node_t));
	if (!out) {
		return NULL;
	}

	out->item = item;
	out->next = NULL;

	return out;
}

void *queuePeek(queue_t *q) {
	if (!q) {
		return NULL;
	}

	return q->head;
}

void *queueGet(queue_t *q) {
	if (!q) {
		return NULL;
	}

	pthread_mutex_lock(&q->mutex);

	if (!q || !q->head) {
		pthread_mutex_unlock(&q->mutex);
		return NULL;
	}

	void *out = q->head->item;
	node_t *oldHead = q->head;
	q->head = q->head->next;
	q->length -= 1;
	free(oldHead);

	if (q->length == 0) {
		q->head = NULL;
		q->tail = NULL;
	}

	pthread_mutex_unlock(&q->mutex);

	return out;
}

int queueAdd(queue_t *q, void *item) {
	if (!q) {
		return 1;
	}

	node_t *newNode = createNode(item);
	if (!newNode) {
		return 2;
	}

	pthread_mutex_lock(&q->mutex);
	if (!q) {
		pthread_mutex_unlock(&q->mutex);
		return 2;
	}

	if (!q->head) {
		q->head = newNode;
		q->tail = newNode;
	}

	q->tail->next = newNode;
	q->tail = newNode;
	q->length += 1;

	pthread_mutex_unlock(&q->mutex);

	return 0;
}

void deleteQueue(queue_t *q) {
	if (!q) {
		return;
	}

	for (size_t i = 0; i < q->length; i++) {
		if (q->freeMethod) {
			q->freeMethod(queueGet(q));
		}
	}

	pthread_mutex_destroy(&q->mutex);
	free(q);
}

