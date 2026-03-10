//------------------------------------------------------------------------------
// Compiler internal shared definitions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/compiler.h>

//------------------------------------------------------------------------------

#define COMPILER_STAGE_FRONT_END "front-end"
#define COMPILER_STAGE_BACK_END "back-end"

#define COMPILER_PHASE_LEX "tokenise source text"
#define COMPILER_PHASE_PARSE "parse tokens into AST"
#define COMPILER_PHASE_IR_GEN "generate IR from AST"
#define COMPILER_PHASE_C_GEN "generate C code from IR"
#define COMPILER_PHASE_C_SAVE "save generated C file"
#define COMPILER_PHASE_C_COMPILE "compile generated C file"

typedef void (*PhaseFn)(void* context);

typedef struct {
    cstr    stage;
    cstr    phase;
    PhaseFn run;
    PhaseFn reset;
} PhaseSpec;

void         compiler_phase_run(const PhaseSpec* phases,
                                usize            phase_count,
                                void*            context,
                                Timing*          timing);
void         compiler_phase_reset_reverse(const PhaseSpec* phases,
                                          usize            phase_count,
                                          void*            context);
TimeDuration compiler_phase_benchmark_single(const PhaseSpec* phases,
                                             usize            phase_count,
                                             usize            phase_index,
                                             void*            context,
                                             u32              warmup_iterations,
                                             u32              timed_iterations);

//------------------------------------------------------------------------------
