#ifndef FIR_MAP_H
#define FIR_MAP_H

#include <fir/fir.h>
#include <stdalign.h>
#include <stdbool.h>

#define MapEntry(T) struct { \
    String key;           \
    bool      used;          \
    T         val;           \
}

#define Map(T) struct { \
    MapEntry(T) *items; \
    size_t       len;   \
    size_t       cap;   \
}

#define map_init(map, init_cap) map_priv_init(map, init_cap)
void map_priv_init(void *map, size_t init_cap);
void map_deinit(void* map);

#define map_insert(map, key, value)                   \
    ((__typeof__((map)->items->val)*)map_priv_insert( \
        map,                                          \
        sizeof(__typeof__((map)->items->val)),        \
        key, value                                    \
    ))
void *map_priv_insert(void* map, size_t size, String key, void* val);

#define map_get(map, key) *(((__typeof__(map))map_priv_get((map), key)))
#define map_get_ref(map, key) ((__typeof__((map)->items->val)*)map_priv_get((map), key))
void *map_priv_get(void* map, String key);

#endif

