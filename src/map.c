#include "fir/map.h"

#include <assert.h>
#include <stdlib.h>


#define GROW_AT 0.6

typedef Map(void*) MapAny;
typedef MapEntry(void*) MapEntryAny;

static size_t hash(String key) {
    unsigned int h = 2166136261;
    for (size_t i = 0; i < key.len; i += 1) {
        h = h ^ ((uint8_t)*(key.ptr + i));
        h = h * 16777619;
    }

    return h;
}

static void resize(void *anymap, size_t size, size_t new_cap) {
    MapAny *map = (MapAny*)anymap;

    MapEntryAny *old_items = (MapEntryAny*)map->items;
    map->items = calloc(sizeof(MapEntry(void*)) * new_cap, 1);

    for (size_t i = 0; i < map->cap; i += 1) {
        if (old_items[i].used) {
            map_priv_insert(map, size, old_items[i].key, map->items[i].val);
        }
    }

    free(old_items);
}

void map_priv_init(void *anymap, size_t init_cap) {
    assert (init_cap >= 8 && "init_cap must be >=8");
    MapAny *map = (MapAny*)anymap;

    map->items = calloc(sizeof(MapEntry(void*)) * init_cap, 1);
    map->cap = init_cap;
    map->len = 0;
}

void map_deinit(void* anymap) {
    MapAny *map = (MapAny*)anymap;

    free(map->items);
    map->len = 0;
    map->cap = 0;
}

void *map_priv_insert(void* anymap, size_t size, String key, void* val) {
    MapAny *map = (MapAny*)anymap;

    if ((float)map->len / (float)map->cap >= GROW_AT) {
        resize(map, size, map->cap * 2);
    }

    size_t start_idx = hash(key);
    for (size_t i = 0; i < map->cap; i += 1) {
        size_t idx = (start_idx + i) % map->cap;

        MapEntryAny *entry = (MapEntryAny*)&map->items[idx];

        bool already_exists = entry->used && string_eq(entry->key, key);
        if (already_exists) {
            return entry->val;
        }

        if (!entry->used) {
            map->len += 1;
            entry->used = true;
            entry->key = key;
            entry->val = val;

            return NULL;
        }
    }

    __builtin_unreachable();
}

void* map_priv_get(void* anymap, String key) {
    MapAny *map = (MapAny*)anymap;

    size_t start_idx = hash(key);
    for(size_t i = 0; i < map->cap; i += 1) {
        size_t idx = (start_idx + i) % map->cap;

        MapEntryAny *entry = (MapEntryAny*)&map->items[idx];

        
        bool exists = entry->used && string_eq(entry->key, key);
        if (exists) {
            return entry->val;
        }
    }

    return NULL;
}

