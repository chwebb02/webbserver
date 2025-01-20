#include "terminal.h"
#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

queue_t *msgQueue = NULL;
pthread_t msgSender;
bool initialized = false;
bool stopSending = false;
unsigned short busyWaitTimer = 1;

void sendMessages() {
    while (1) {
        if (stopSending) {
            break;
        }

        char *msg = queueGet(msgQueue);
        if (!msg) {
            sleep(busyWaitTimer);
            continue;
        }

        printf("\n%s\n>", msg);
        free(msg);
    }
}

int initTerminal(config_t *conf) {
    if (initialized) {
        return 1;
    }


    msgQueue = createQueue((void (*)(void *)) free);
    if (!msgQueue) {
        return 2;
    }

    pthread_create(&msgSender, NULL, (void *(*)(void *)) sendMessages, NULL);
    busyWaitTimer = conf->threadpoolBusyWaitTimer;
    initialized = true;

    return 0;
}

int terminalSendMessage(const char *msg) {
    if (!msg || !initialized) {
        return 1;
    }

    char *buffer = malloc((strlen(msg) + 1) * sizeof(char));
    if (!buffer) {
        return 2;
    }
    strncpy(buffer, msg, strlen(msg));

    if (queueAdd(msgQueue, buffer)) {
        free(buffer);
        return 3;
    }

    return 0;
}

void stopMsgSender() {
    stopSending = true;
    pthread_join(msgSender, NULL);
}

void deleteTerminal() {
    stopMsgSender();
    deleteQueue(msgQueue);
    stopSending = false;
    initialized = false;
}