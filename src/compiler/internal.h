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

#define COMPILER_PHASE_LEX "tokenise source text"
#define COMPILER_PHASE_PARSE "parse tokens into AST"
#define COMPILER_PHASE_SEMA "analyse AST semantics"
#define COMPILER_PHASE_IR_GEN "generate IR from AST"
#define COMPILER_PHASE_C_GEN "generate C code from IR"
#define COMPILER_PHASE_C_SAVE "save generated C file"
#define COMPILER_PHASE_C_COMPILE "compile generated C file"

//------------------------------------------------------------------------------
