//------------------------------------------------------------------------------
// Hashing FNV-1a algorithm
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

u64 hash_fnv1a(const void* data, usize size)
{
    const u8* bytes = (const u8*)data;
    u64       hash  = 14695981039346656037ull; // FNV offset basis

    for (usize i = 0; i < size; i++) {
        hash ^= bytes[i];
        hash *= 1099511628211ull; // FNV prime
    }

    return hash;
}

u64 hash_fnv1a_string(string str) { return hash_fnv1a(str.data, str.count); }
