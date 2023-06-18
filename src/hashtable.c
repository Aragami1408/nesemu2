#include "hashtable.h"

static size_t __hash_table_index(hashtable_t *ht, const char *key) {
	return (ht->hash(key, strlen(key)) % ht->size);
}

hashtable_t *hashtable_init(u32 size, hashfunction *hf, cleanup_func *cf) {
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	ht->size = size;
	ht->hash = hf;
	if(cf) {
		ht->cleanup = cf;
	}
	else {
		ht->cleanup = free;
	}
	ht->elements = calloc(sizeof(hashtable_entry_t), ht->size);
	return ht;
}

void hashtable_free(hashtable_t *ht) {
	for(u32 i = 0; i < ht->size; i++) {
		while(ht->elements[i]) {
			hashtable_entry_t *tmp = ht->elements[i];
			ht->elements[i] = ht->elements[i]->next;
			free(tmp->key);
			ht->cleanup(tmp->object);
			free(tmp);
		}
	}
	free(ht->elements);
	free(ht);
}

void hashtable_print(hashtable_t *ht) {
	printf("Start Table\n");
	for(u32 i = 0; i < ht->size; i++) {
		if(ht->elements[i] == NULL) {
			// do we want to print out null entries?
			printf("\t%i\t---\n", i);
		}
		else {
			printf("\t%i\t\n", i);
			hashtable_entry_t *tmp = ht->elements[i];
			while(tmp != NULL) {
				printf("\"%s\"(%p) - ", tmp->key, tmp->object);
				tmp = tmp->next;
			}
			printf("\n");
		}
	}

	printf("End Table\n");
}

bool hashtable_insert(hashtable_t *ht, const char *key, void *obj) {
	if(key == NULL || obj == NULL) return false;
	size_t index = __hash_table_index(ht, key);

	if(hashtable_lookup(ht, key) != NULL) return false;

	// create a new entry
	hashtable_entry_t *e = malloc(sizeof(hashtable_entry_t));
	e->object = obj;
	// e->key = malloc(strlen(key)+1);
	// strcpy(e->key, key);
	e->key = strdup(key);

	// insert entry
	e->next = ht->elements[index];
	ht->elements[index] = e;
	return true;
}

void *hashtable_lookup(hashtable_t *ht, const char *key) {
	if(key == NULL || ht == NULL) return NULL;
	size_t index = __hash_table_index(ht, key);

	hashtable_entry_t *tmp = ht->elements[index];
	while(tmp != NULL && strcmp(tmp->key, key) != 0) {
		tmp = tmp->next;
	}
	if(tmp == NULL) return NULL;
	return tmp->object;
}

void *hashtable_delete(hashtable_t *ht, const char *key) {
	if(key == NULL || ht == NULL) return NULL;
	size_t index = __hash_table_index(ht, key);

	hashtable_entry_t *tmp = ht->elements[index];
	hashtable_entry_t *prev = NULL;
	while(tmp != NULL && strcmp(tmp->key, key) != 0) {
		prev = tmp;
		tmp = tmp->next;
	}
	if(tmp == NULL) return NULL;
	if(prev == NULL) {
		// deleting the head of the list
		ht->elements[index] = tmp->next;
	}
	else {
		// deleting from the middle/end of the list
		prev->next = tmp->next;
	}
	void *result = tmp->object;
	free(tmp);
	return result;
}

