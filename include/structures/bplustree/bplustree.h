#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "structures/list.h"

// Macro to define a typed B+ tree.
//
// name                   - prefix for generated types/functions
// entry_type             - user struct type that contains the key (intrusive entry)
// key_type               - type of key (e.g. int, long, etc.)
// key_member             - member name inside entry_type that is the key
// ORDER                  - max number of children for internal nodes (>=3). leaf capacity
// = ORDER-1 CMP(a,b)     - macro/function comparing two keys: returns <0 if a<b, 0 if equal, >0 if a>b
//
#define DEFINE_BTREE(name, entry_type, key_type, key_member, ORDER, CMP)       \
                                                                               \
  enum { name##_ORDER = (ORDER), name##_MAX_KEYS = (ORDER) - 1 };              \
                                                                               \
  /* internal node */                                                          \
  struct name##_node {                                                         \
    bool                is_leaf;                                               \
    int                 nkeys;                                                 \
    key_type            keys[name##_MAX_KEYS]; /* separators */                \
    struct name##_node *children[(ORDER)];     /* children count = nkeys+1 */  \
    /* leaf-specific */                                                        \
    entry_type      *leaf_entries[name##_MAX_KEYS];                            \
    struct list_head leaf_link; /* link into leaf-level doubly-linked list */  \
  };                                                                           \
                                                                               \
  struct name {                                                                \
    struct name##_node *root;                                                  \
    struct list_head    leaves; /* head of leaf list */                        \
  };                                                                           \
                                                                               \
  /* allocate node */                                                          \
  static inline struct name##_node *name##_node_alloc(void)                    \
  {                                                                            \
    struct name##_node *n = malloc(sizeof(*n));                                \
    if (!n) return NULL;                                                       \
    n->is_leaf = true;                                                         \
    n->nkeys   = 0;                                                            \
    INIT_LIST_HEAD(&n->leaf_link);                                             \
    memset(n->children, 0, sizeof(n->children));                               \
    memset(n->leaf_entries, 0, sizeof(n->leaf_entries));                       \
    return n;                                                                  \
  }                                                                            \
                                                                               \
  static inline void name##_init(struct name *t)                               \
  {                                                                            \
    t->root = NULL;                                                            \
    INIT_LIST_HEAD(&t->leaves);                                                \
  }                                                                            \
                                                                               \
  /* find leaf node for key (return node and index where key <= separator) */  \
  static inline struct name##_node *name##_find_leaf(struct name *t,           \
                                                     key_type     key)         \
  {                                                                            \
    struct name##_node *n = t->root;                                           \
    if (!n) return NULL;                                                       \
    while (!n->is_leaf) {                                                      \
      int i = 0;                                                               \
      while (i < n->nkeys && CMP(key, n->keys[i]) >= 0)                        \
        i++; /* go right while key >= key[i] */                                \
      n = n->children[i];                                                      \
    }                                                                          \
    return n;                                                                  \
  }                                                                            \
                                                                               \
  /* search for key -> return entry_type* or NULL */                           \
  static inline entry_type *name##_search(struct name *t, key_type key)        \
  {                                                                            \
    struct name##_node *leaf = name##_find_leaf(t, key);                       \
    if (!leaf) return NULL;                                                    \
    for (int i = 0; i < leaf->nkeys; ++i) {                                    \
      if (CMP(key, leaf->leaf_entries[i]->key_member) == 0)                    \
        return leaf->leaf_entries[i];                                          \
    }                                                                          \
    return NULL;                                                               \
  }                                                                            \
                                                                               \
  /* helper: insert into leaf assumed not full */                              \
  static inline void name##_leaf_insert_nofull(struct name##_node *leaf,       \
                                               entry_type         *e)          \
  {                                                                            \
    key_type k = e->key_member;                                                \
    int      i = leaf->nkeys - 1;                                              \
    /* shift right until place found */                                        \
    while (i >= 0 && CMP(leaf->leaf_entries[i]->key_member, k) > 0) {          \
      leaf->leaf_entries[i + 1] = leaf->leaf_entries[i];                       \
      i--;                                                                     \
    }                                                                          \
    leaf->leaf_entries[i + 1] = e;                                             \
    leaf->nkeys++;                                                             \
  }                                                                            \
                                                                               \
  /* split leaf: leaf is full, create new_leaf and move half entries */        \
  static inline struct name##_node *name##_split_leaf(                         \
      struct name##_node *leaf)                                                \
  {                                                                            \
    int mid = (name##_MAX_KEYS + 1)                                            \
              / 2; /* ceil half stays in left? we'll move right half */        \
    struct name##_node *right = name##_node_alloc();                           \
    if (!right) return NULL;                                                   \
    right->is_leaf = true;                                                     \
    right->nkeys   = 0;                                                        \
    /* move entries */                                                         \
    int j = 0;                                                                 \
    for (int i = mid; i < leaf->nkeys; ++i)                                    \
      right->leaf_entries[j++] = leaf->leaf_entries[i];                        \
    right->nkeys = j;                                                          \
    /* shrink left */                                                          \
    leaf->nkeys = mid;                                                         \
    /* wire leaf list: insert right after leaf */                              \
    list_add_tail(&right->leaf_link, &leaf->leaf_link);                        \
    return right;                                                              \
  }                                                                            \
                                                                               \
  /* split internal node */                                                    \
  static inline struct name##_node *name##_split_internal(                     \
      struct name##_node *node,                                                \
      key_type           *up_key)                                              \
  {                                                                            \
    int                 mid   = node->nkeys / 2; /* middle key will go up */   \
    struct name##_node *right = malloc(sizeof(*right));                        \
    if (!right) return NULL;                                                   \
    right->is_leaf = false;                                                    \
    right->nkeys   = 0;                                                        \
    /* copy keys after mid (exclude mid) */                                    \
    int j = 0;                                                                 \
    for (int i = mid + 1; i < node->nkeys; ++i)                                \
      right->keys[j++] = node->keys[i];                                        \
    right->nkeys = j;                                                          \
    /* children */                                                             \
    for (int i = 0; i <= j; ++i)                                               \
      right->children[i] = node->children[mid + 1 + i];                        \
    /* shrink left */                                                          \
    node->nkeys = mid;                                                         \
    *up_key     = node->keys[mid];                                             \
    return right;                                                              \
  }                                                                            \
                                                                               \
  /* insert entry into tree (simplified B+ insertion) */                       \
  static inline int name##_insert(struct name *t, entry_type *entry)           \
  {                                                                            \
    if (!t->root) {                                                            \
      /* create root as leaf */                                                \
      struct name##_node *r = name##_node_alloc();                             \
      if (!r) return -1;                                                       \
      r->is_leaf = true;                                                       \
      r->nkeys   = 0;                                                          \
      /* link leaf to leaf list head */                                        \
      list_add_tail(&r->leaf_link, &t->leaves);                                \
      t->root = r;                                                             \
    }                                                                          \
    /* descent, keep stack of nodes */                                         \
    struct name##_node *path[64];                                              \
    int                 path_pos = 0;                                          \
    struct name##_node *n        = t->root;                                    \
    while (!n->is_leaf) {                                                      \
      path[path_pos++] = n;                                                    \
      int i            = 0;                                                    \
      while (i < n->nkeys && CMP(entry->key_member, n->keys[i]) >= 0) i++;     \
      n = n->children[i];                                                      \
    }                                                                          \
    path[path_pos++] = n;                                                      \
    /* if leaf full, split up the path until leaf not full */                  \
    if (n->nkeys == name##_MAX_KEYS) {                                         \
      /* split leaf and propagate */                                           \
      struct name##_node *left  = n;                                           \
      struct name##_node *right = name##_split_leaf(left);                     \
      if (!right) return -1;                                                   \
      /* create new parent if needed */                                        \
      if (path_pos == 1) { /* root was leaf */                                 \
        struct name##_node *newroot = malloc(sizeof(*newroot));                \
        if (!newroot) return -1;                                               \
        newroot->is_leaf     = false;                                          \
        newroot->nkeys       = 1;                                              \
        newroot->keys[0]     = right->leaf_entries[0]->key_member;             \
        newroot->children[0] = left;                                           \
        newroot->children[1] = right;                                          \
        t->root              = newroot;                                        \
      }                                                                        \
      else {                                                                   \
        /* insert separator into parent; we will handle propagation            \
         * iteratively */                                                      \
        int                 insert_pos = path_pos - 2; /* parent index */      \
        struct name##_node *parent     = path[insert_pos];                     \
        /* we will insert a key = first key of right into parent; if parent    \
         * full - split later */                                             \
        key_type sep = right->leaf_entries[0]->key_member;                     \
        /* shift parent's keys/children to insert */                           \
        int k = parent->nkeys - 1;                                             \
        while (k >= 0 && CMP(parent->keys[k], sep) > 0) {                      \
          parent->keys[k + 1]     = parent->keys[k];                           \
          parent->children[k + 2] = parent->children[k + 1];                   \
          k--;                                                                 \
        }                                                                      \
        parent->keys[k + 1]     = sep;                                         \
        parent->children[k + 2] = right;                                       \
        parent->nkeys++;                                                       \
        /* propagate splits upward if parent overflows */                      \
        for (int pi = insert_pos;                                              \
             pi >= 0 && path[pi]->nkeys > name##_MAX_KEYS;                     \
             --pi) {                                                           \
          struct name##_node *over = path[pi];                                 \
          key_type            upkey;                                           \
          struct name##_node *rnode = name##_split_internal(over, &upkey);     \
          if (!rnode) return -1;                                               \
          if (pi == 0) { /* make new root */                                   \
            struct name##_node *newroot = malloc(sizeof(*newroot));            \
            if (!newroot) return -1;                                           \
            newroot->is_leaf     = false;                                      \
            newroot->nkeys       = 1;                                          \
            newroot->keys[0]     = upkey;                                      \
            newroot->children[0] = over;                                       \
            newroot->children[1] = rnode;                                      \
            t->root              = newroot;                                    \
            break;                                                             \
          }                                                                    \
          else {                                                               \
            struct name##_node *pp = path[pi - 1];                             \
            /* insert upkey into pp at proper position */                      \
            int ii = pp->nkeys - 1;                                            \
            while (ii >= 0 && CMP(pp->keys[ii], upkey) > 0) {                  \
              pp->keys[ii + 1]     = pp->keys[ii];                             \
              pp->children[ii + 2] = pp->children[ii + 1];                     \
              ii--;                                                            \
            }                                                                  \
            pp->keys[ii + 1]     = upkey;                                      \
            pp->children[ii + 2] = rnode;                                      \
            pp->nkeys++;                                                       \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    /* now find leaf again (may have changed) and insert */                    \
    struct name##_node *leaf = name##_find_leaf(t, entry->key_member);         \
    if (!leaf) return -1;                                                      \
    /* insert into leaf (leaf guaranteed not full now) */                      \
    name##_leaf_insert_nofull(leaf, entry);                                    \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  /* iterate over all entries in order: callback(entry*, ctx) */               \
  static inline void name##_iterate(struct name *t,                            \
                                    void (*cb)(entry_type *, void *),          \
                                    void *ctx)                                 \
  {                                                                            \
    struct list_head *h = &t->leaves;                                          \
    for (struct list_head *p = h->next; p != h; p = p->next) {                 \
      struct name##_node *leaf                                                 \
          = container_of(p, struct name##_node, leaf_link);                    \
      for (int i = 0; i < leaf->nkeys; ++i) cb(leaf->leaf_entries[i], ctx);    \
    }                                                                          \
  }
