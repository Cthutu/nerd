//------------------------------------------------------------------------------
// C Generation Implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>

static const char g_cgen_prelude[] = {
#embed "../../../data/prelude.c"
    , 0};

static const char g_cgen_epilogue[] = {
#embed "../../../data/epilogue.c"
    , 0};

//------------------------------------------------------------------------------
// C generation helpers

// Emit the current indentation prefix.

void cgen_start_line(CGen* cgen)
{
    for (int i = 0; i < cgen->indent_level; ++i) {
        arena_format(&cgen->arena, "    ");
    }
}

//------------------------------------------------------------------------------
// Append a text fragment to the generated C buffer.

void cgen_add(CGen* cgen, cstr line) { arena_format(&cgen->arena, "%s", line); }

//------------------------------------------------------------------------------
// Append raw bytes to the generated C buffer.

void cgen_add_bytes(CGen* cgen, const char* text, usize count)
{
    if (count == 0) {
        return;
    }

    char* dst = (char*)arena_alloc(&cgen->arena, count);
    memcpy(dst, text, count);
}

//------------------------------------------------------------------------------
// Append a text fragment followed by a newline.

void cgen_addn(CGen* cgen, cstr line)
{
    cgen_add(cgen, line);
    arena_format(&cgen->arena, "\n");
}

//------------------------------------------------------------------------------
// Append an indented line followed by a newline.

void cgen_add_line(CGen* cgen, cstr line)
{
    cgen_start_line(cgen);
    cgen_addn(cgen, line);
}

//------------------------------------------------------------------------------
// Increase indentation for subsequent lines.

void cgen_indent(CGen* cgen) { ++cgen->indent_level; }

//------------------------------------------------------------------------------
// Decrease indentation for subsequent lines.

void cgen_dedent(CGen* cgen)
{
    ASSERT(cgen->indent_level > 0, "Indent level cannot be negative");
    --cgen->indent_level;
}

//------------------------------------------------------------------------------
// Prologue

void cgen_add_prologue(CGen* cgen)
{
    cgen_add_bytes(cgen, g_cgen_prelude, sizeof(g_cgen_prelude) - 1);
    cgen_addn(cgen, "");
}

//------------------------------------------------------------------------------
// Epilogue

void cgen_add_epilogue(CGen* cgen)
{
    cgen_addn(cgen, "");
    cgen_add_bytes(cgen, g_cgen_epilogue, sizeof(g_cgen_epilogue) - 1);
}

//------------------------------------------------------------------------------
// C generation for IR instructions

//------------------------------------------------------------------------------
// Render a C symbol name for a Nerd function.

void cgen_add_symbol_name(CGen* cgen, u32 symbol_handle)
{
    cgen_add(cgen, "$");
    arena_format(
        &cgen->arena, STRINGP, STRINGV(lex_symbol(cgen->lexer, symbol_handle)));
}

//------------------------------------------------------------------------------
// Render an IR value into C syntax.

void cgen_add_value(CGen* cgen, const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        arena_format(&cgen->arena, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_INTEGER:
        arena_format(&cgen->arena, "%lld", value->value.integer);
        break;
    case IR_VALUE_SYMBOL:
        cgen_add_symbol_name(cgen, (u32)value->value.integer);
        break;
    default:
        kill("Unknown IR value kind: %u", value->kind);
        break;
    }
}

//------------------------------------------------------------------------------
// Emit a C assignment from an IR assignment instruction.

void cgen_add_assign(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add(cgen, "int ");
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL,
               "Expected assignable lvalue");
    }
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// Emit a C return from an IR return instruction.

void cgen_add_return(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    cgen_add(cgen, "return ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// Emit a C unary expression from an IR unary instruction.

void cgen_add_unary(CGen* cgen, const IrInstruction* instr, cstr op)
{
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add(cgen, "int ");
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL,
               "Expected assignable lvalue");
    }
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add(cgen, op);
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// Emit a C binary expression from an IR binary instruction.

void cgen_add_binary(CGen* cgen, const IrInstruction* instr, cstr op)
{
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add(cgen, "int ");
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL,
               "Expected assignable lvalue");
    }
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, op);
    cgen_add_value(cgen, &instr->rvalue[1]);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// Emit a C global declaration for a top-level constant binding.

void cgen_add_global(CGen* cgen, const IrInstruction* instr)
{
    ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL, "Expected global symbol");
    cgen_start_line(cgen);
    cgen_add(cgen, "int ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// C generation

void cgen_generate(CGen* cgen, const Ir* ir)
{
    bool has_init_section = false;
    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        if (ir->instructions[i].op == IR_OP_INIT_START) {
            has_init_section = true;
            break;
        }
    }

    if (!has_init_section) {
        cgen_add_line(cgen, "int init() {");
        cgen_indent(cgen);
        cgen_add_line(cgen, "return 0;");
        cgen_dedent(cgen);
        cgen_add_line(cgen, "}");
    }

    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        const IrInstruction* instr = &ir->instructions[i];
        switch (instr->op) {
        case IR_OP_GLOBAL:
            cgen_add_global(cgen, instr);
            break;
        case IR_OP_INIT_START:
            cgen_add_line(cgen, "int init() {");
            cgen_indent(cgen);
            break;
        case IR_OP_INIT_END:
            cgen_add_line(cgen, "return 0;");
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_FN_START:
            cgen_start_line(cgen);
            cgen_add(cgen, "int ");
            cgen_add_symbol_name(cgen, (u32)instr->lvalue.value.integer);
            cgen_addn(cgen, "() {");
            cgen_indent(cgen);
            break;
        case IR_OP_FN_END:
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_ASSIGN:
            cgen_add_assign(cgen, instr);
            break;
        case IR_OP_NEGATE:
            cgen_add_unary(cgen, instr, "-");
            break;
        case IR_OP_ADD:
            cgen_add_binary(cgen, instr, " + ");
            break;
        case IR_OP_SUBTRACT:
            cgen_add_binary(cgen, instr, " - ");
            break;
        case IR_OP_MULTIPLY:
            cgen_add_binary(cgen, instr, " * ");
            break;
        case IR_OP_DIVIDE:
            cgen_add_binary(cgen, instr, " / ");
            break;
        case IR_OP_MODULO:
            cgen_add_binary(cgen, instr, " % ");
            break;
        case IR_OP_RETURN:
            cgen_add_return(cgen, instr);
            break;
        default:
            eprn("Unknown IR operation: %u", instr->op);
            abort();
            break;
        }
    }
}

//------------------------------------------------------------------------------

// Build the generated C buffer from IR.

CGen cgen_init(const Ir* ir, const Lexer* lexer)
{
    CGen cgen = {.lexer = lexer};
    arena_init(&cgen.arena);

    cgen_add_prologue(&cgen);
    cgen.generated_start = cgen.arena.cursor;
    cgen_generate(&cgen, ir);
    cgen.generated_end = cgen.arena.cursor;
    cgen_add_epilogue(&cgen);

    return cgen;
}

void cgen_done(CGen* cgen)
{
    arena_done(&cgen->arena);
    *cgen = (CGen){0};
}
