#ifndef MAP_H
#define MAP_H

#include <fir.h>
#include <stdalign.h>
#include <stdbool.h>


#define MapEntry(T) struct { \
    FirString key;              \
    bool used;               \
    T val;                   \
}

#define Map(T) struct { \
    MapEntry(T) *items; \
    size_t len;         \
    size_t cap;         \
}

#define map_init(map, init_cap) map_priv_init(map, init_cap)
void map_priv_init(void *map, size_t init_cap);
void map_deinit(void* map);

#define map_insert(map, key, value) do { \
    __typeof__(map) tmp = value;         \
    map_priv_insert(                     \
        (void**)&map,                    \
        sizeof(__typeof__(*map)),        \
        key,                             \
        tmp                              \
    );                                   \
} while(0)
bool map_priv_insert(void* map, FirString key, void* value);

#define map_try_get(map, key) *try(((__typeof__(map))map_priv_get(map, key)))
#define map_get_ref(map, key) ((__typeof__(map))internal__map_get(map, key))
void* map_priv_get(void* map, FirString key);

#endif

