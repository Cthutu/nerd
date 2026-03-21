//------------------------------------------------------------------------------
// Hash map implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

internal const u64   MAP_HASH_PRESENT_MASK = 0x8000000000000000ull;
internal const usize MAP_MIN_CAPACITY      = 8;

internal usize map_next_pow2(usize value)
{
    usize result = MAP_MIN_CAPACITY;
    while (result < value) {
        result <<= 1;
    }
    return result;
}

internal u64 map_store_hash(string key)
{
    return hash_fnv1a_string(key) | MAP_HASH_PRESENT_MASK;
}

internal usize map_probe_distance(usize slot, u64 hash, usize mask)
{
    return (slot + (mask + 1) - (hash & mask)) & mask;
}

internal bool map_key_eq(string a, string b)
{
    if (a.count != b.count) {
        return false;
    }

    if (a.data == b.data) {
        return true;
    }

    return memcmp(a.data, b.data, a.count) == 0;
}

internal u8* map_value_at(Map* map, usize index)
{
    return map->values + index * map->value_size;
}

internal string map_key_dup(Map* map, string key)
{
    if (key.count == 0) {
        return (string){0};
    }

    u8* copy = (u8*)arena_alloc(&map->key_arena, key.count);
    memcpy(copy, key.data, key.count);
    return string_from(copy, key.count);
}

internal bool map_should_compact(const Map* map)
{
    return map->key_bytes_dead >= KB(4) &&
           map->key_bytes_dead > map->key_bytes_live;
}

internal usize map_capacity(const Map* map)
{
    if (map->hashes == NULL) {
        return 0;
    }

    return mem_size(map->hashes) / sizeof(map->hashes[0]);
}

internal void map_reserve(Map* map, usize capacity)
{
    capacity    = map_next_pow2(MAX(capacity, MAP_MIN_CAPACITY));

    map->hashes = ARRAY_ALLOC(u64, capacity);
    map->keys   = ARRAY_ALLOC(string, capacity);
    map->values = ARRAY_ALLOC(u8, capacity * map->value_size);

    memset(map->hashes, 0, capacity * sizeof(u64));
    memset(map->keys, 0, capacity * sizeof(string));
}

internal bool
map_insert_hashed(Map* map, u64 hash, string key, const void* value)
{
    usize mask = map_capacity(map) - 1;
    usize idx  = hash & mask;
    usize dist = 0;

    u8  stack_value[256];
    u8* carry_value = stack_value;
    if (map->value_size > sizeof(stack_value)) {
        carry_value = ARRAY_ALLOC(u8, map->value_size);
    }
    memcpy(carry_value, value, map->value_size);

    for (;;) {
        u64 slot_hash = map->hashes[idx];
        if (slot_hash == 0) {
            map->hashes[idx] = hash;
            map->keys[idx]   = key;
            memcpy(map_value_at(map, idx), carry_value, map->value_size);
            map->count++;

            if (carry_value != stack_value) {
                ARRAY_FREE(carry_value);
            }
            return true;
        }

        if (slot_hash == hash && map_key_eq(map->keys[idx], key)) {
            memcpy(map_value_at(map, idx), carry_value, map->value_size);

            if (carry_value != stack_value) {
                ARRAY_FREE(carry_value);
            }
            return false;
        }

        usize slot_dist = map_probe_distance(idx, slot_hash, mask);
        if (slot_dist < dist) {
            u8  stack_swap[256];
            u8* swap_value = stack_swap;
            if (map->value_size > sizeof(stack_swap)) {
                swap_value = ARRAY_ALLOC(u8, map->value_size);
            }

            memcpy(swap_value, map_value_at(map, idx), map->value_size);
            memcpy(map_value_at(map, idx), carry_value, map->value_size);
            memcpy(carry_value, swap_value, map->value_size);

            if (swap_value != stack_swap) {
                ARRAY_FREE(swap_value);
            }

            u64    old_hash  = map->hashes[idx];
            string old_key   = map->keys[idx];
            map->hashes[idx] = hash;
            map->keys[idx]   = key;
            hash             = old_hash;
            key              = old_key;
            dist             = slot_dist;
        }

        idx  = (idx + 1) & mask;
        dist = dist + 1;
    }
}

internal void map_rebuild(Map* map, usize new_capacity)
{
    Map fresh        = {0};
    fresh.value_size = map->value_size;
    arena_init(&fresh.key_arena);
    map_reserve(&fresh, new_capacity);

    usize capacity = map_capacity(map);
    for (usize i = 0; i < capacity; i++) {
        if (map->hashes[i] != 0) {
            string key = map_key_dup(&fresh, map->keys[i]);
            map_insert_hashed(
                &fresh, map->hashes[i], key, map_value_at(map, i));
            fresh.key_bytes_live += key.count;
        }
    }

    ARRAY_FREE(map->hashes);
    ARRAY_FREE(map->keys);
    ARRAY_FREE(map->values);
    arena_done(&map->key_arena);

    *map = fresh;
}

internal void map_maybe_grow(Map* map)
{
    usize capacity = map_capacity(map);
    if (capacity == 0) {
        map_reserve(map, MAP_MIN_CAPACITY);
        return;
    }

    if ((map->count + 1) * 4 >= capacity * 3) {
        map_rebuild(map, capacity << 1);
    }
}

internal void map_maybe_compact(Map* map)
{
    if (map_should_compact(map)) {
        map_rebuild(map, map_capacity(map));
    }
}

void map_iter_init(MapIter* iter) { iter->index = 0; }

bool map_next(Map* map, MapIter* iter, string* out_key, void** out_value)
{
    usize capacity = map_capacity(map);
    while (iter->index < capacity) {
        usize index = iter->index++;
        if (map->hashes[index] != 0) {
            if (out_key != NULL) {
                *out_key = map->keys[index];
            }
            if (out_value != NULL) {
                *out_value = map_value_at(map, index);
            }
            return true;
        }
    }

    return false;
}

void _map_init(Map* map, usize value_size, usize initial_capacity)
{
    memset(map, 0, sizeof(*map));
    map->value_size = value_size;
    arena_init(&map->key_arena);
    map_reserve(map, initial_capacity);
}

void map_done(Map* map)
{
    ARRAY_FREE(map->hashes);
    ARRAY_FREE(map->keys);
    ARRAY_FREE(map->values);
    arena_done(&map->key_arena);
    memset(map, 0, sizeof(*map));
}

void map_clear(Map* map)
{
    usize capacity = map_capacity(map);
    if (capacity == 0) {
        return;
    }

    memset(map->hashes, 0, capacity * sizeof(u64));
    memset(map->keys, 0, capacity * sizeof(string));
    map->count          = 0;
    map->key_bytes_live = 0;
    map->key_bytes_dead = 0;
    arena_reset(&map->key_arena);
}

bool map_insert(Map* map, string key, const void* value)
{
    ASSERT(value != NULL, "map_insert requires a non-null value pointer");

    bool out_created = false;
    memcpy(map_entry(map, key, &out_created), value, map->value_size);
    return out_created;
}

void* map_find(Map* map, string key)
{
    usize capacity = map_capacity(map);
    if (map->count == 0 || capacity == 0) {
        return NULL;
    }

    u64   hash = map_store_hash(key);
    usize mask = capacity - 1;
    usize idx  = hash & mask;
    usize dist = 0;

    for (;;) {
        u64 slot_hash = map->hashes[idx];
        if (slot_hash == 0) {
            return NULL;
        }

        usize slot_dist = map_probe_distance(idx, slot_hash, mask);
        if (slot_dist < dist) {
            return NULL;
        }

        if (slot_hash == hash && map_key_eq(map->keys[idx], key)) {
            return map_value_at(map, idx);
        }

        idx  = (idx + 1) & mask;
        dist = dist + 1;
    }
}

bool map_delete(Map* map, string key)
{
    usize capacity = map_capacity(map);
    if (map->count == 0 || capacity == 0) {
        return false;
    }

    u64   hash = map_store_hash(key);
    usize mask = capacity - 1;
    usize idx  = hash & mask;
    usize dist = 0;

    for (;;) {
        u64 slot_hash = map->hashes[idx];
        if (slot_hash == 0) {
            return false;
        }

        usize slot_dist = map_probe_distance(idx, slot_hash, mask);
        if (slot_dist < dist) {
            return false;
        }

        if (slot_hash == hash && map_key_eq(map->keys[idx], key)) {
            map->key_bytes_live -= map->keys[idx].count;
            map->key_bytes_dead += map->keys[idx].count;

            usize hole = idx;
            usize next = (idx + 1) & mask;

            while (map->hashes[next] != 0 &&
                   map_probe_distance(next, map->hashes[next], mask) > 0) {
                map->hashes[hole] = map->hashes[next];
                map->keys[hole]   = map->keys[next];
                memcpy(map_value_at(map, hole),
                       map_value_at(map, next),
                       map->value_size);
                hole = next;
                next = (next + 1) & mask;
            }

            map->hashes[hole] = 0;
            map->keys[hole]   = (string){0};
            memset(map_value_at(map, hole), 0, map->value_size);
            map->count--;
            map_maybe_compact(map);
            return true;
        }

        idx  = (idx + 1) & mask;
        dist = dist + 1;
    }
}

void* map_entry(Map* map, string key, bool* out_created)
{
    if (out_created != NULL) {
        *out_created = false;
    }

    void* existing = map_find(map, key);
    if (existing != NULL) {
        return existing;
    }

    map_maybe_grow(map);

    string owned_key = map_key_dup(map, key);
    map->key_bytes_live += owned_key.count;

    u64   hash = map_store_hash(key);
    usize mask = map_capacity(map) - 1;
    usize idx  = hash & mask;
    usize dist = 0;

    for (;;) {
        u64 slot_hash = map->hashes[idx];
        if (slot_hash == 0) {
            map->hashes[idx] = hash;
            map->keys[idx]   = owned_key;
            memset(map_value_at(map, idx), 0, map->value_size);
            map->count++;
            if (out_created != NULL) {
                *out_created = true;
            }
            return map_value_at(map, idx);
        }

        if (slot_hash == hash && map_key_eq(map->keys[idx], key)) {
            return map_value_at(map, idx);
        }

        usize slot_dist = map_probe_distance(idx, slot_hash, mask);
        if (slot_dist < dist) {
            u8    stack_zero[256] = {0};
            u8*   carry_value     = stack_zero;
            void* entry_value     = map_value_at(map, idx);
            if (map->value_size > sizeof(stack_zero)) {
                carry_value = ARRAY_ALLOC(u8, map->value_size);
                memset(carry_value, 0, map->value_size);
            }

            for (;;) {
                memcpy(carry_value, map_value_at(map, idx), map->value_size);
                memset(map_value_at(map, idx), 0, map->value_size);

                u64    old_hash  = map->hashes[idx];
                string old_key   = map->keys[idx];
                map->hashes[idx] = hash;
                map->keys[idx]   = owned_key;

                hash             = old_hash;
                key              = old_key;
                owned_key        = old_key;

                idx              = (idx + 1) & mask;
                dist             = slot_dist + 1;

                for (;;) {
                    slot_hash = map->hashes[idx];
                    if (slot_hash == 0) {
                        map->hashes[idx] = hash;
                        map->keys[idx]   = owned_key;
                        memcpy(map_value_at(map, idx),
                               carry_value,
                               map->value_size);
                        map->count++;
                        if (out_created != NULL) {
                            *out_created = true;
                        }

                        if (carry_value != stack_zero) {
                            ARRAY_FREE(carry_value);
                        }

                        return entry_value;
                    }

                    slot_dist = map_probe_distance(idx, slot_hash, mask);
                    if (slot_dist < dist) {
                        u8  stack_swap[256];
                        u8* swap_value = stack_swap;
                        if (map->value_size > sizeof(stack_swap)) {
                            swap_value = ARRAY_ALLOC(u8, map->value_size);
                        }

                        memcpy(swap_value,
                               map_value_at(map, idx),
                               map->value_size);
                        memcpy(map_value_at(map, idx),
                               carry_value,
                               map->value_size);
                        memcpy(carry_value, swap_value, map->value_size);

                        if (swap_value != stack_swap) {
                            ARRAY_FREE(swap_value);
                        }

                        old_hash         = map->hashes[idx];
                        old_key          = map->keys[idx];
                        map->hashes[idx] = hash;
                        map->keys[idx]   = owned_key;
                        hash             = old_hash;
                        key              = old_key;
                        owned_key        = old_key;
                        dist             = slot_dist;
                    }

                    idx  = (idx + 1) & mask;
                    dist = dist + 1;
                }
            }
        }

        idx  = (idx + 1) & mask;
        dist = dist + 1;
    }
}

//------------------------------------------------------------------------------
