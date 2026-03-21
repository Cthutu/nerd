//------------------------------------------------------------------------------
// C generation module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> def: _CRT_SECURE_NO_WARNINGS

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
string cgen_render(const CGen* cgen, Arena* arena);
void   cgen_save(const CGen* cgen, const char* path);

void cgen_dump(const CGen* cgen);
