//------------------------------------------------------------------------------
// String interner implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <intern/intern.h>

//------------------------------------------------------------------------------

#define INTERN_SEED 0x7f4a7c1506fe123dull
#define INTERN_EXPECTED_SIZE 1024
#define INTERN_MAX_LOAD_FACTOR 0.85f
#define INTERN_SIZE_MASK (INTERN_EXPECTED_SIZE - 1)
#define INTERN_ALIGNMENT 16

//------------------------------------------------------------------------------
// Hashing

internal inline u64 int_rot_left_64(u64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

internal inline u64 int_split_mix_64_next(u64* x)
{
    u64 z = (*x += 0x9e3779b97f4a7c15ull);
    z     = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z     = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}

internal inline u64 int_hash_bytes_64(u8* data, u8 len, u64 seed)
{
    u64 s = seed;
    u64 h = 0;

    // Body hashing
    while (len >= 8) {
        u64 v;
        memcpy(&v, data, 8);
        data += 8;
        len -= 8;
        s ^= v;
        u64 m = int_split_mix_64_next(&s);
        h ^= int_rot_left_64(m, 17);
        h += 0x9ddfea08eb382d69ull;
    }

    // Tail hashing
    u64 t = 0;
    for (u32 i = 0; i < len; i++) {
        t |= ((u64)data[i]) << (i * 8);
    }
    s ^= t;
    h ^= int_split_mix_64_next(&s);
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdull;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ull;
    h ^= h >> 33;

    return h ? h : 0x9e3779b97f4a7c15ull;
}

internal inline u64 int_next_power_of_two(u64 v)
{
#if ARCH_X86_64 || (COMPILER_GCC || COMPILER_CLANG)
    if (v < 2) {
        return 2;
    }
    return 1ull << (64 - __builtin_clzll(v - 1));
#else
    if (v < 2) {
        return 2;
    }
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
#endif // ARCH_X86_64
}

//------------------------------------------------------------------------------
// Interner

void _intern_init(Interner* interner, InternInitParams params)
{
    // Ensure all fields start from a known state, especially the slots pointer
    // that dynamic array helpers expect to be NULL.
    *interner = (Interner){0};

    arena_init(&interner->intern_arena);
    interner->max_load_factor = params.max_load_factor ? params.max_load_factor
                                                       : INTERN_MAX_LOAD_FACTOR;
    interner->seed            = params.seed ? params.seed : INTERN_SEED;

    u64 cap_needed =
        (u64)((f64)params.expected_size / interner->max_load_factor) + 2;
    interner->capacity      = int_next_power_of_two(cap_needed);
    interner->capacity_mask = interner->capacity - 1;
    interner->count         = 0;

    array_requires_size(interner->slots, interner->capacity);
    memset(interner->slots, 0, sizeof(InternSlot) * interner->capacity);
}

void intern_done(Interner* interner)
{
    array_free(interner->slots);
    arena_done(&interner->intern_arena);
}

void intern_leak(Interner* interner) { array_leak(interner->slots); }

internal inline bool intern_load_exceeded(Interner* interner)
{
    return (f64)interner->count >
           interner->max_load_factor * (f64)interner->capacity;
}

string intern_find(Interner* interner, string str)
{
    if (interner->capacity == 0) {
        return (string){0};
    }

    u64 h   = int_hash_bytes_64(str.data, (u8)str.count, interner->seed);
    u64 i   = h & interner->capacity_mask;
    u64 psl = 0;

    for (;;) {
        const InternSlot* slot = &interner->slots[i];
        if (slot->hash == 0) {
            // Empty slot - string not found
            return (string){0};
        }
        if (slot->hash == h && slot->str->len == str.count &&
            memcmp(slot->str->str, str.data, str.count) == 0) {
            // Found matching string
            return (string){.data = slot->str->str, .count = slot->str->len};
        }
        if (psl > slot->psl) {
            // Robin Hood invariant violated - string not in table
            return (string){0};
        }
        i = (i + 1) & interner->capacity_mask;
        ++psl;
    }
}

internal void intern_maybe_grow(Interner* interner)
{
    if (!intern_load_exceeded(interner)) {
        return;
    }

    u64 old_cap                 = interner->capacity;
    u64 new_cap                 = old_cap ? old_cap * 2 : 8;
    u64 new_mask                = new_cap - 1;

    Array(InternSlot) new_slots = NULL;
    array_requires_size(new_slots, new_cap);
    memset(new_slots, 0, sizeof(InternSlot) * new_cap);

    // Reinsert existing entries using Robin Hood hashing
    for (u64 j = 0; j < old_cap; ++j) {
        InternSlot* old_slot = &interner->slots[j];
        if (old_slot->hash == 0) {
            continue;
        }

        InternSlot entry = *old_slot;
        u64        i     = entry.hash & new_mask;
        entry.psl        = 0;

        for (;;) {
            InternSlot* slot = &new_slots[i];
            if (slot->hash == 0) {
                // Slot is empty - insert here
                *slot     = entry;
                slot->psl = entry.psl;
                break;
            }

            if (entry.psl > slot->psl) {
                // Robin Hood: displace the richer entry
                InternSlot tmp = *slot;
                *slot          = entry;
                slot->psl      = entry.psl;
                entry          = tmp;
            }

            i = (i + 1) & new_mask;
            ++entry.psl;
        }
    }

    array_free(interner->slots);
    interner->slots         = new_slots;
    interner->capacity      = new_cap;
    interner->capacity_mask = new_mask;
}

string intern_add(Interner* interner, string str)
{
    // Check if we need to grow the table
    intern_maybe_grow(interner);

    u64 h   = int_hash_bytes_64(str.data, (u8)str.count, interner->seed);
    u64 i   = h & interner->capacity_mask;
    u64 psl = 0;

    InternedString* insert_str        = NULL;
    InternedString* original_str      = NULL;
    bool            original_inserted = false;
    u64             insert_hash       = h;
    u8              insert_len        = (u8)str.count;
    u8*             insert_data       = str.data;

    for (;;) {
        InternSlot* slot = &interner->slots[i];
        if (slot->hash == 0) {
            // Empty slot - insert here

            // Allocate InternedString for what we're inserting if not done yet
            if (!insert_str) {
                usize num_bytes_to_alloc = sizeof(InternedString) + insert_len;
                insert_str =
                    (InternedString*)arena_alloc_align(&interner->intern_arena,
                                                       num_bytes_to_alloc,
                                                       INTERN_ALIGNMENT);
                insert_str->len  = insert_len;
                insert_str->hash = insert_hash;
                memcpy(insert_str->str, insert_data, insert_len);
            }

            slot->hash = insert_hash;
            slot->psl  = psl;
            slot->str  = insert_str;

            interner->count++;
            InternedString* result =
                original_inserted ? original_str : insert_str;
            return (string){.data = result->str, .count = result->len};
        }

        if (slot->hash == insert_hash && slot->str->len == insert_len &&
            memcmp(slot->str->str, insert_data, insert_len) == 0) {
            // Found matching string - return it
            if (!original_inserted) {
                return (string){.data  = slot->str->str,
                                .count = slot->str->len};
            }
            return (string){.data  = original_str->str,
                            .count = original_str->len};
        }

        if (psl > slot->psl) {
            // Robin Hood: displace the richer entry

            // Allocate InternedString for what we're inserting if not done yet
            if (!insert_str) {
                usize num_bytes_to_alloc = sizeof(InternedString) + insert_len;
                insert_str =
                    (InternedString*)arena_alloc_align(&interner->intern_arena,
                                                       num_bytes_to_alloc,
                                                       INTERN_ALIGNMENT);
                insert_str->len  = insert_len;
                insert_str->hash = insert_hash;
                memcpy(insert_str->str, insert_data, insert_len);
            }

            // Swap what we want to insert with what's in the slot
            u64             tmp_hash = slot->hash;
            u64             tmp_psl  = slot->psl;
            InternedString* tmp_str  = slot->str;

            slot->hash               = insert_hash;
            slot->psl                = psl;
            slot->str                = insert_str;

            if (!original_inserted) {
                original_inserted = true;
                original_str      = insert_str;
            }

            // Now we need to insert the displaced entry
            insert_hash = tmp_hash;
            insert_len  = tmp_str->len;
            insert_data = tmp_str->str;
            insert_str  = tmp_str;
            psl         = tmp_psl;
        }

        i = (i + 1) & interner->capacity_mask;
        ++psl;
    }
}

string intern_cstr(Interner* interner, cstr str)
{
    return intern_add(interner, string_from_cstr(str));
}

InternedString* intern_get_info(string str)
{
    if (str.data == NULL) {
        return NULL;
    }

    InternedString* interned_str =
        (InternedString*)(str.data - offsetof(InternedString, str));
    return interned_str;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
