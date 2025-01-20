#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

typedef struct threadpoolStruct threadpool_t;

/// @brief Create a threadpool where each thread is running threadFunction
/// @param threadCount The number of threads in the pool
/// @param threadFunction The function that each thread is running concurrently
/// @param threadpoolFunctionArgs The arguments to supply to threadFunction
/// @return The threadpool object
threadpool_t *createThreadpool(unsigned short threadCount, unsigned short busyWaitTimer, void *(*threadFunction)(void *), void *threadpoolFunctionArgs);

/// @brief Change the function that the threads in the pool are running
/// @param tp The threadpool object
/// @param threadFunction The function to change to
/// @param threadpoolFunctionArgs The arguments to supply to the new threadFunction
void threadpoolChangeFunction(threadpool_t *tp, void *(*threadFunction)(void *), void *threadpoolFunctionArgs);

/// @brief Delete a threadpool and cancel all threads
/// @param tp The threadpool object
void deleteThreadpool(threadpool_t *tp);

#endif /* _THREADPOOL_H_ */