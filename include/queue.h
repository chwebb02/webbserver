#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct queueStruct queue_t;

/// @brief Create a new queue
/// @param freeMethod Method to free remaining items in queue if deleteQueue is called
/// @return The queue object
queue_t *createQueue(void (*freeMethod)(void *));

/// @brief Peek at the top item on the queue
/// @param q The queue object
/// @return The item at the head of the queue, or NULL on error
void *queuePeek(queue_t *q);

/// @brief Get the item at the head of the list and deletes it from the queue
/// @param q The queue object
/// @return The item at the head of the list, or NULL on error
/// @note Queue returns ownership of item pointer 
void *queueGet(queue_t *q);

/// @brief Adds an item to the queue
/// @param q The queue object
/// @param item Item to add to the queue
/// @return 0 on success, or an error code
/// @note Queue takes ownership of item pointer
int queueAdd(queue_t *q, void *item);

/// @brief Free all memory owned by the queue object
/// @param q The queue object
/// @note Ensure that all threads that are waiting on this queue have been cancelled before calling
void deleteQueue(queue_t *q);

#endif /* _QUEUE_H_ */