//------------------------------------------------------------------------------
// String interner module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef struct {
    u64 hash;
    u8  len;
    u8  str[0];
} InternedString;

typedef struct {
    u64             hash; // 0 = empty
    InternedString* str;  // Pointer to string in intern arena
    u64             psl;  // Probe sequence length
} InternSlot;

typedef struct {
    Array(InternSlot) slots;
    Arena intern_arena;
    u64   capacity;        // Always a power of two
    u64   capacity_mask;   // capacity - 1
    u64   count;           // Number of live entries
    f64   max_load_factor; // How full should it get before we grow
    void* slots_mark;      // Arena mark for slots
    u64   seed;            // Hash seed
} Interner;

typedef struct {
    u64 expected_size;
    f64 max_load_factor;
    u64 seed;
} InternInitParams;

void _intern_init(Interner* interner, InternInitParams params);
void intern_done(Interner* interner);
void intern_leak(Interner* interner);

#define intern_init(interner, ...)                                             \
    _intern_init((interner), (InternInitParams){__VA_ARGS__})

string intern_cstr(Interner* interner, cstr str);
string intern_find(Interner* interner, string str);
string intern_add(Interner* interner, string str);

InternedString* intern_get_info(string str);

#define INTERN_NOTFOUND (-1u)
