#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/errno.h>
#include <sys/random.h>

#include "structures/bplustree/int_int_bplustree.h"

#define OBJECTS_SIZE 5

unsigned int getrandom_uint(void) {
  unsigned int result;
  ssize_t ret;

  ret = getrandom(&result, sizeof(result), 0);
  if (ret != sizeof(result)) {
    fprintf(stderr, "getrandom error: returned %zd, errno = %d\n", ret, errno);
    return 0;
  }

  return result;
}

int main(void) {
  intinttree tree;
  intinttree_init(&tree);

  IntIntBPlusTree *objs[OBJECTS_SIZE];
  for (int i = 0; i < OBJECTS_SIZE; ++i) {
    objs[i] = malloc(sizeof(*objs[i]));
    // super simple hash function
    objs[i]->key   = i;
    objs[i]->value = (int)getrandom_uint();
    intinttree_insert(&tree, objs[i]);
  }

  printf("Iterate:\n");
  intinttree_iterate(&tree, printIntIntBPlusTree, NULL);

  // test find
  int              k = getrandom_uint() % OBJECTS_SIZE;
  IntIntBPlusTree *r = intinttree_search(&tree, k);
  if (r)
    printf("Found %d -> %d\n", k, r->value);
  else
    printf("Not found %d\n", k);

  return 0;
}
