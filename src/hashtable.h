#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"


typedef u64 (hashfunction)(const char *, size_t);
typedef void (cleanup_func)(void *);
typedef struct hashtable hashtable_t;
typedef struct hashtable_entry hashtable_entry_t;

struct hashtable_entry {
	char *key;
	void *object;
	hashtable_entry_t *next;
};

struct hashtable {
	u32 size;
	hashfunction *hash;
	hashtable_entry_t **elements;
	cleanup_func *cleanup;
};

hashtable_t *hashtable_init(u32 size, hashfunction *hf, cleanup_func *cf);
void hashtable_free(hashtable_t *ht);
void hashtable_print(hashtable_t *ht);
bool hashtable_insert(hashtable_t *ht, const char *key, void *obj);
void *hashtable_lookup(hashtable_t *ht, const char *key);
void *hashtable_delete(hashtable_t *ht, const char *key);



#endif
