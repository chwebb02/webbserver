#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <sys/types.h>

typedef struct hashmapStruct hashmap_t;

typedef size_t (*hashFunction_t)(hashmap_t *, const char *);

/// @brief Create a hashmap that holds 
/// @param size The maximum size of the hashmap domain
/// @param hashFunction Custom hash function (will use library defined function if NULL)
/// @param freeItemFunc Function to free objects in the hashmap on deletion (does not take ownership if freeItemFunc is NULL)
/// @return A hashmap object or NULL on failure
hashmap_t *createHashmap(size_t domainSize, hashFunction_t hashFunc, void (*freeItemFunc)(void *));

/// @brief Search for an item in the hashmap by key
/// @param map The hashmap object
/// @param key The key of the item searched for
/// @return The item searched for, or NULL if it does not exit or there is an error
void *hashmapLookup(hashmap_t *map, const char *key);

/// @brief Insert a new key-value pair into a hashmap
/// @param map The hashmap object
/// @param key The key of the insertion
/// @param value The value of the insertion
/// @return 0 on success, or an error code
/// @note THe hashmap takes ownership of the value, but not the key, only if freeItemFunc is not NULL
int hashmapInsert(hashmap_t *map, const char *key, void *value);

/// @brief Release all of the memory associated with a hashmap
/// @param map The hashmap object
void deleteHashmap(hashmap_t *map);

#endif /* _HASHMAP_H_ */