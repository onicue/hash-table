#include "hmap.h"
#include <stdio.h>
#include <string.h>

#define HM_EMPTY_NODE NULL

static inline void
_hmap_free_node(hmap_node* node){
  free((void*)node->key);
  free(node->value);
  free(node);
}

#if HM_REFACTOR_FLAG
static inline void
_hmap_refactor_check(hmap** table){
  if((float)(*table)->size * HM_REFACTOR_THRESHOLD <= (*table)->total_elements){
    hmap_resize(table, (*table)->element_count * HM_RESIZE_RATE);
  }
}
#endif

static uint32_t
murmurhash (const char *key, uint32_t len, uint32_t seed) {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = 0;
  uint32_t k = 0;
  uint8_t *d = (uint8_t *) key; // 32 bit extract from `key'
  const uint32_t *chunks = NULL;
  const uint8_t *tail = NULL; // tail - last 8 bytes
  int i = 0;
  int l = len / 4; // chunk length

  h = seed;

  chunks = (const uint32_t *) (d + l * 4); // body
  tail = (const uint8_t *) (d + l * 4); // last 8 byte chunk of `key'

  // for each 4 byte chunk of `key'
  for (i = -l; i != 0; ++i) {
    // next 4 byte chunk of `key'
    k = chunks[i];

    // encode next 4 byte chunk of `key'
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    // append to hash
    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }

  k = 0;

  // remainder
  switch (len & 3) { // `len % 4'
    case 3: k ^= (tail[2] << 16);
    case 2: k ^= (tail[1] << 8);

    case 1:
      k ^= tail[0];
      k *= c1;
      k = (k << r1) | (k >> (32 - r1));
      k *= c2;
      h ^= k;
  }

  h ^= len;

  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);

  return h;
}

static inline uint32_t
_hmap_compute_index(const char* key, uint table_size){
  return murmurhash(key, strlen(key), HM_SEED) % table_size;
}

inline static hmap_node*
_hmap_init_node(const char* key, void* value, size_t value_size){
  hmap_node* node = malloc(sizeof(hmap_node));
  if(node == HM_EMPTY_NODE){
    fprintf(stderr, "ERROR: node initialization error\n");
    return NULL;
  }

  node->key = strdup(key);
  if (!node->key) {
    free(node);
    fprintf(stderr, "ERROR: key initialization error\n");
    return NULL;
  }

  node->value = malloc(value_size);
  if (!node->value) {
    free((void*)node->key);
    free(node);
    fprintf(stderr, "ERROR: Value allocation error\n");
    return NULL;
  }

  memcpy(node->value, value, value_size);

  node->next = HM_EMPTY_NODE;
  node->state = 0;

  return node;
}

static int
_hmap_create_node(hmap* table, const char* key, void* value) {
  uint32_t index = _hmap_compute_index(key, table->size);
  hmap_node** node = &table->chains[index];

  while (*node != HM_EMPTY_NODE) {
    if (!strcmp(key, (*node)->key) && !(*node)->state) {
      return 0;  // Duplicate key found
    } else {
      node = &(*node)->next;
    }
  }

  *node = _hmap_init_node(key, value, table->value_size);
  if (*node == HM_EMPTY_NODE) {
    return 0;  // Node initialization error
  }

  table->element_count++;
  table->total_elements++;
  return 1;
}

hmap*
hmap_init(uint default_size, size_t value_size){
  if (default_size < 10) {
    fprintf(stderr, "Warning: Small size for hash table, size set to 10\n");
    default_size = 10;
  }

  hmap* table = malloc(sizeof(hmap));
  if(!table){
    fprintf(stderr, "Error: Unable to allocate memory for hmap");
    return HM_EMPTY_NODE;
  }

  table->value_size = value_size;
  table->size = default_size;
  table->element_count = 0;
  table->total_elements = 0;

  table->chains = calloc(default_size, sizeof(hmap_node*));
  if(!table->chains){
    fprintf(stderr, "Error: Unable to allocate memory for hmap_node array");
    return HM_EMPTY_NODE;
  }

  for (uint i = 0; i < default_size; i++) {
    table->chains[i] = HM_EMPTY_NODE;
  }
  return table;
}

hmap_node*
hmap_find(hmap* table, const char* key){
  uint32_t index = _hmap_compute_index(key, table->size);
  hmap_node* node = table->chains[index];

  while (node != HM_EMPTY_NODE) {
    if ((!strcmp(key, node->key)) && (!node->state)) {
      return node;
    }
    node = node->next;
  }

  return HM_EMPTY_NODE;
}

int
hmap_insert(hmap* table, const char *key, void *value){
  int insertStatus = _hmap_create_node(table, key, value);

#if HM_REFACTOR_FLAG
  _hmap_refactor_check(&table);
#endif

  return insertStatus;
}

void*
hmap_get(hmap* table, const char *key){
  hmap_node* node = hmap_find(table, key);
  return node == HM_EMPTY_NODE ? NULL : node->value;
}

int
hmap_change(hmap* table, const char *key, void *value){
  hmap_node* node = hmap_find(table, key);
  return hmap_change_by_node(node, value, table->value_size);
}

int
hmap_change_by_node(hmap_node* node, void* value, size_t value_size){
  if(node == HM_EMPTY_NODE){
    return 0;
  }
  memcpy(node->value, value, value_size);
  return 1;
}


int
hmap_resize(hmap** table, uint new_size) {
  if ((*table) == NULL) {
    fprintf(stderr, "Error: Attempting to resize a null hash table.\n");
    return 0;
  }

  if (new_size == 0) {
    fprintf(stderr, "Error: Invalid size for the new hash table.\n");
    return 0;
  }

  hmap* new_map = hmap_init(new_size, (*table)->value_size);
  if (!new_map) {
    fprintf(stderr, "Error: Failed to initialize hash table inside hmap_resize.\n");
    return 0;
  }

  for (uint i = 0; i < (*table)->size; i++) {
    hmap_node* node = (*table)->chains[i];
    while (node != HM_EMPTY_NODE) {
      if (!node->state) {
        int created_node_state = _hmap_create_node(new_map, node->key, node->value);
        if (!created_node_state) {
          fprintf(stderr, "Error: Failed to create a node during resizing.\n");
          return 0;
        }
      }
      node = node->next;
    }
  }

  hmap_free(*table);
  *table = new_map;
  return 1;
}

int
hmap_delete(hmap *table, const char *key){
  hmap_node* node = hmap_find(table, key);
  return hmap_delete_by_node(table, node);
}


int
hmap_delete_by_node(hmap* table, hmap_node *node) {
  if (node == HM_EMPTY_NODE) {
    return 0;  // Node not found
  }

  node->state = 1;
  table->element_count--;
  return 1;  // Node successfully deleted
}

void
hmap_free(hmap *table){
  for(uint i = 0; i<table->size; i++){
    hmap_node* node = table->chains[i];
    while(node != HM_EMPTY_NODE){
      hmap_node* next_node = node->next;
      _hmap_free_node(node);
      node = next_node;
    }
  }
  free(table->chains);
  free(table);
}

