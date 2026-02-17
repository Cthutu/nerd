//------------------------------------------------------------------------------
// Compiler internal shared definitions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/compiler.h>

//------------------------------------------------------------------------------

#define COMPILER_STAGE_FRONT_END "front-end"
#define COMPILER_PHASE_LEX "tokenise source text"

#define PHASE_DO(timing, stage_name, phase_name, stmt)                         \
    do {                                                                       \
        if ((timing) != NULL) {                                                \
            ThreadTimePoint _start = thread_time_now();                        \
            do {                                                               \
                stmt;                                                          \
            } while (0);                                                       \
            ThreadTimePoint _end = thread_time_now();                          \
            timing_add((timing),                                               \
                       (stage_name),                                           \
                       (phase_name),                                           \
                       thread_time_elapsed(_start, _end));                     \
        } else {                                                               \
            do {                                                               \
                stmt;                                                          \
            } while (0);                                                       \
        }                                                                      \
    } while (0)

//------------------------------------------------------------------------------
