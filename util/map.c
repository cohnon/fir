#include "map.h"

#include <assert.h>
#include <stdlib.h>


typedef Map(void*) MapAny;
typedef MapEntry(void*) MapEntryAny;

static size_t hash(FirString key) {
    unsigned int h = 2166136261;
    for (size_t i = 0; i < key.len; i += 1) {
        h = h ^ ((uint8_t)*(key.ptr + i));
        h = h * 16777619;
    }

    return h;
}

static void resize(void *anymap, size_t new_cap) {
    MapAny *map = (MapAny*)anymap;

    MapEntryAny *old_items = (MapEntryAny*)map->items;
    map->items = calloc(sizeof(MapEntry(void*)) * new_cap, 1);

    for (size_t i = 0; i < map->cap; i += 1) {
        if (old_items[i].used) {
            map_priv_insert(map, old_items[i].key, map->items[i].val);
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

bool map_priv_insert(void* anymap, FirString key, void* val) {
    MapAny *map = (MapAny*)anymap;

    if (map->len * 5 >= map->cap * 4) {
        resize(map, map->cap * 2);
    }

    size_t start_idx = hash(key);
    for (size_t i = 0; i < map->cap; i += 1) {
        size_t idx = (start_idx + i) % map->cap;

        MapEntryAny *entry = (MapEntryAny*)&map->items[idx];

        bool already_exists = entry->used && fir_string_eq(entry->key, key);
        if (already_exists) {
            return false;
        }

        if (!entry->used) {
            map->len += 1;
            entry->used = true;
            entry->key = key;
            entry->val = val;

            return true;
        }
    }

    return false;
}

void* map_priv_get(void* anymap, FirString key) {
    MapAny *map = (MapAny*)anymap;

    size_t start_idx = hash(key);
    for(size_t i = 0; i < map->cap; i += 1) {
        size_t idx = (start_idx + i) % map->cap;

        MapEntryAny *entry = (MapEntryAny*)&map->items[idx];

        
        bool exists = entry->used && fir_string_eq(entry->key, key);
        if (exists) {
            return entry->val;
        }
    }

    return NULL;
}

