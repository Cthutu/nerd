//------------------------------------------------------------------------------
// Build module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/cgen/cgen.h>
#include <compiler/compiler.h>
#include <compiler/ir/ir.h>
#include <compiler/lexer/lexer.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------

typedef struct {
    Lexer lexer;
    Ast   ast;
    Sema  sema;
    Ir    ir;
} FrontEndState;

typedef struct {
    CGen cgen;
} BackEndState;
