#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <stdio.h>

struct threadpoolStruct {
	pthread_t *threads;
	unsigned short threadCount;
	void *(*threadFunction)(void *);
	void *threadpoolFunctionArgs;
	unsigned short busyWaitTimer;
	bool exitFlag;
};

void threadpoolIdleFunction(threadpool_t *tp) {
	if (!tp) {
		return;
	}

	while (1) {
		tp->threadFunction(tp->threadpoolFunctionArgs);
		sleep(tp->busyWaitTimer);

		if (tp->exitFlag) {
			break;
		}
	}
}

threadpool_t *createThreadpool(unsigned short threadCount, unsigned short busyWaitTimer, void *(*threadFunction)(void *), void *threadpoolFunctionArgs) {
	if (threadCount < 1 || !threadFunction) {
		return NULL;
	}

	threadpool_t *out = malloc(sizeof(threadpool_t));
	if (!out) {
		return NULL;
	}

	out->threads = malloc(threadCount * sizeof(pthread_t));
	if (!out->threads) {
		free(out);
		return NULL;
	}

	out->busyWaitTimer = busyWaitTimer;
	out->threadFunction = threadFunction;
	out->threadpoolFunctionArgs = threadpoolFunctionArgs;
	out->exitFlag = false;

	out->threadCount = threadCount;
	for (unsigned short i = 0; i < threadCount; i++) {
		pthread_create(out->threads + i, NULL, (void *(*)(void *)) threadpoolIdleFunction, out);
	}

	return out;
}

void exitAllThreads(threadpool_t *tp) {
	if (!tp) {
		return;
	}

	tp->exitFlag = true;

	for (unsigned int i = 0; i < tp->threadCount; i++) {
		pthread_join(tp->threads[i], NULL);
	}

	tp->exitFlag = false;
}

void threadpoolChangeFunction(threadpool_t *tp, void *(*threadFunction)(void *), void *threadpoolFunctionArgs) {
	if (!tp || !threadFunction) {
		return;
	}

	exitAllThreads(tp);

	for (unsigned short i = 0; i < tp->threadCount; i++) {
		pthread_create(tp->threads + i, NULL, threadFunction, threadpoolFunctionArgs);
	}
}


void deleteThreadpool(threadpool_t *tp) {
	if (!tp) {
		return;
	}

	exitAllThreads(tp);
	free(tp->threads);
	free(tp);
}