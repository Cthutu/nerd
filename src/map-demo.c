//------------------------------------------------------------------------------
// Map Demo code
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core

#include <core/core.h>

//------------------------------------------------------------------------------

DEF_MAP(AgesMap, i32);

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    AgesMap ages;
    AgesMap_init(&ages, 16);

    AgesMap_insert(&ages, s("Alice"), 30);
    AgesMap_insert(&ages, s("Bob"), 25);
    AgesMap_insert(&ages, s("Charlie"), 41);
    AgesMap_insert(&ages, s("Bob"), 26);

    i32* alice = AgesMap_find(&ages, s("Alice"));
    i32* bob   = AgesMap_find(&ages, s("Bob"));
    i32* dave  = AgesMap_find(&ages, s("Dave"));

    prn("count: %d", (int)AgesMap_count(&ages));
    prn("Alice: %d", alice ? *alice : -1);
    prn("Bob: %d", bob ? *bob : -1);
    prn("Dave: %s", dave ? "present" : "missing");

    AgesMap_delete(&ages, s("Alice"));
    prn("Alice after delete: %s",
        AgesMap_find(&ages, s("Alice")) ? "present" : "missing");

    AgesMap_clear(&ages);
    prn("count after clear: %d", (int)AgesMap_count(&ages));

    AgesMap_done(&ages);
    return 0;
}
