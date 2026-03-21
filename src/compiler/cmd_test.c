//------------------------------------------------------------------------------
// Test command
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cmd_internal.h>

//------------------------------------------------------------------------------

int compiler_cmd_test(const NerdTestConfig* config)
{
    UNUSED(config);

    prn("nerd test: command dispatch is working");
    return 0;
}

//------------------------------------------------------------------------------
