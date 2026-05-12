//------------------------------------------------------------------------------
// Compiler internal shared definitions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/build/build.h>
#include <compiler/compiler.h>

//------------------------------------------------------------------------------

#define COMPILER_STAGE_FRONT_END "front-end"
#define COMPILER_STAGE_BACK_END "back-end"
#define COMPILER_STAGE_FORMATTER "formatter"
#define COMPILER_STAGE_LSP "lsp"

#define COMPILER_PHASE_LEX "tokenise source text"
#define COMPILER_PHASE_PARSE "parse tokens into AST"
#define COMPILER_PHASE_SEMA "analyse AST semantics"
#define COMPILER_PHASE_HIR_GEN "generate HIR from sema"
#define COMPILER_PHASE_LLVM_RENDER "render module LLVM"
#define COMPILER_PHASE_LLVM_COMBINE "combine LLVM text"
#define COMPILER_PHASE_RUNTIME_OBJECT "write runtime object"
#define COMPILER_PHASE_LINK "link executable"
#define COMPILER_PHASE_FORMAT_SOURCE "format source"
#define COMPILER_PHASE_LSP_ANALYSE "analyse document"

static inline MemoryStats compiler_memory_profile_begin(void)
{
    return mem_stats_snapshot();
}

static inline void compiler_memory_profile_end(cstr        stage,
                                               cstr        phase,
                                               MemoryStats before)
{
    if (!mem_stats_profile_enabled()) {
        return;
    }

    MemoryStats after = mem_stats_snapshot();
    mem_stats_print_delta(stage, phase, mem_stats_delta(before, after));
}

//------------------------------------------------------------------------------
