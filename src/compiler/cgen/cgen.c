//------------------------------------------------------------------------------
// C Generation Implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>

//------------------------------------------------------------------------------
// C generation helpers

void cgen_start_line(CGen* cgen)
{
    for (int i = 0; i < cgen->indent_level; ++i) {
        arena_format(&cgen->arena, "    ");
    }
}

void cgen_add(CGen* cgen, cstr line) { arena_format(&cgen->arena, "%s", line); }

void cgen_addn(CGen* cgen, cstr line)
{
    cgen_add(cgen, line);
    arena_format(&cgen->arena, "\n");
}

void cgen_add_line(CGen* cgen, cstr line)
{
    cgen_start_line(cgen);
    cgen_addn(cgen, line);
}

void cgen_indent(CGen* cgen) { ++cgen->indent_level; }

void cgen_dedent(CGen* cgen)
{
    ASSERT(cgen->indent_level > 0, "Indent level cannot be negative");
    --cgen->indent_level;
}

//------------------------------------------------------------------------------
// Prologue

void cgen_add_prologue(CGen* cgen)
{
    cgen_add_line(cgen, "//");
    cgen_add_line(cgen, "// Generated C code");
    cgen_add_line(cgen, "//\n");
}

//------------------------------------------------------------------------------
// Epilogue

void cgen_add_epilogue(CGen* cgen)
{
    // No epilogue for now, but this is where we would add any necessary
    // closing braces or cleanup code if needed in the future.
    UNUSED(cgen);
}

//------------------------------------------------------------------------------
// C generation for IR instructions

void cgen_add_value(CGen* cgen, const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        arena_format(&cgen->arena, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_INTEGER:
        arena_format(&cgen->arena, "%u", (u32)value->value.integer);
        break;
    default:
        eprn("Unknown IR value kind: %u", value->kind);
        abort();
        break;
    }
}

void cgen_add_assign(CGen* cgen, const IrInstruction* instr)
{
    ASSERT(instr->lvalue.kind == IR_VALUE_VARIABLE, "Expected variable lvalue");
    cgen_start_line(cgen);
    cgen_add(cgen, "int ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

void cgen_add_return(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    cgen_add(cgen, "return ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

//------------------------------------------------------------------------------
// C generation

void cgen_generate(CGen* cgen, const Ir* ir)
{
    cgen_add_line(cgen, "int main() {");
    cgen_indent(cgen);

    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        const IrInstruction* instr = &ir->instructions[i];
        switch (instr->op) {
        case IR_OP_ASSIGN:
            cgen_add_assign(cgen, instr);
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

    cgen_dedent(cgen);
    cgen_add_line(cgen, "}");
}

//------------------------------------------------------------------------------

CGen cgen_init(const Ir* ir)
{
    CGen cgen = {0};
    arena_init(&cgen.arena);

    cgen_add_prologue(&cgen);
    cgen_generate(&cgen, ir);
    cgen_add_epilogue(&cgen);

    return cgen;
}

void cgen_done(CGen* cgen)
{
    arena_done(&cgen->arena);
    *cgen = (CGen){0};
}
