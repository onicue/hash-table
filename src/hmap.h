#ifndef HMAP_H
#define HMAP_H

#include <stdlib.h>
#include <stdint.h>


#ifndef HM_SEED
#define HM_SEED 31
#endif

#ifndef HM_RESIZE_RATE
#define HM_RESIZE_RATE 1.2
#endif


#ifndef HM_REFACTOR_THRESHOLD
#define HM_REFACTOR_THRESHOLD 0.75
#endif

#ifndef HM_REFACTOR_FLAG
#define HM_REFACTOR_FLAG 1
#endif

typedef struct hmap_node{
  const char* key;
  void* value;
  int state;
  struct hmap_node* next;
} hmap_node;

typedef struct {
  size_t value_size;
  uint size;
uint element_count;
  uint total_elements;
  hmap_node** chains;
} hmap;

int
hmap_insert(hmap* table, const char* key, void* value);

int
hmap_delete(hmap* table, const char* key);

int
hmap_change(hmap* table, const char* key, void* value);

int
hmap_change_by_node(hmap_node* node, void* value, size_t value_size);

hmap*
hmap_init(uint default_size, size_t value_size);

hmap_node*
hmap_find(hmap* table, const char* key);

hmap*
hmap_resize(hmap* table, uint new_size);

void
hmap_free(hmap* table);

void*
hmap_get(hmap* tabel, const char* key);

int
hmap_delete_by_node(hmap* table, hmap_node* node);
#endif
