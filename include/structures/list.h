#pragma once

#ifndef container_of
#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

typedef struct list_head {
  struct list_head *next, *prev;
} list_head;
typedef struct list_head list_head;

static inline void INIT_LIST_HEAD(list_head *head) {
  head->next = head->prev = head;
}

static inline void list_add_tail(list_head *n, list_head *head) {
  n->prev          = head->prev;
  n->next          = head;
  head->prev->next = n;
  head->prev       = n;
}

static inline void list_del(list_head *n) {
  n->prev->next = n->next;
  n->next->prev = n->prev;
  INIT_LIST_HEAD(n);
}
