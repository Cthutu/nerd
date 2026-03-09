//------------------------------------------------------------------------------
// C generation module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ir/ir.h>

//------------------------------------------------------------------------------
// C generation API

typedef struct {
    Arena arena;
    int   indent_level;
} CGen;

CGen cgen_init(const Ir* ir);
void cgen_done(CGen* cgen);
void cgen_save(const CGen* cgen, const char* path);

void cgen_dump(const CGen* cgen);
