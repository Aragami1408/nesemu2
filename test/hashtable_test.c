#include "minunit.h"
#include "../src/types.h"
#include "../src/hashtable.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 4096

u64 hash(const char *name, size_t length) {
	u64 hash_value = 0;
	for(u64 i = 0; i < length; i++) {
		hash_value += name[i];
		hash_value = (hash_value * name[i]);
	}
	return hash_value;
}

void test_setup(void) {
}

void test_teardown(void) {
	/* Nothing */
}


int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;
	return 0;
}

