//------------------------------------------------------------------------------
// Intermediate Representation (IR) module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/sema/sema.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    IR_OP_GLOBAL,
    IR_OP_INIT_START,
    IR_OP_INIT_END,
    IR_OP_FN_START,
    IR_OP_FN_END,
    IR_OP_LOCAL,
    IR_OP_ASSIGN,
    IR_OP_CALL,
    IR_OP_CAST,
    IR_OP_STRING_RESET,
    IR_OP_STRING_START,
    IR_OP_STRING_APPEND,
    IR_OP_STRING_FINISH,
    IR_OP_NEGATE,
    IR_OP_ADD,
    IR_OP_SUBTRACT,
    IR_OP_MULTIPLY,
    IR_OP_DIVIDE,
    IR_OP_MODULO,
    IR_OP_RETURN,
} IrOperation;

typedef enum : u8 {
    IR_VALUE_NONE,
    IR_VALUE_VARIABLE,
    IR_VALUE_LOCAL,
    IR_VALUE_INTEGER,
    IR_VALUE_SYMBOL,
    IR_VALUE_BUILTIN,
    IR_VALUE_STRING,
} IrValueKind;

typedef struct {
    IrValueKind kind;
    union {
        i64 integer;
    } value;
} IrValue;

typedef struct {
    IrOperation op;
    IrValue     lvalue;
    IrValue     rvalue[2];
} IrInstruction;

typedef struct {
    Array(IrInstruction) instructions;
    Array(string) strings;
    Array(SemaType) types;
    Arena arena;
} Ir;

//------------------------------------------------------------------------------
// IR generation API

Ir     ir_generate(const Lexer* lex, const Ast* ast, const Sema* sema);
void   ir_done(Ir* ir);
string ir_render(const Ir* ir, const Lexer* lexer, Arena* arena);
void   ir_save(const Ir* ir, const Lexer* lexer, cstr path);
void   ir_dump(const Ir* ir, const Lexer* lexer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
