//------------------------------------------------------------------------------
// IR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ir/ir.h>
#include <stdio.h>

//------------------------------------------------------------------------------

internal void ir_render_value(StringBuilder* sb, const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        sb_format(sb, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_INTEGER:
        sb_format(sb, "%u", (u32)value->value.integer);
        break;
    case IR_VALUE_NONE:
        break;
    default:
        sb_append_cstr(sb, "<unknown>");
        break;
    }
}

string ir_render(const Ir* ir, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    for (usize i = 0; i < array_count(ir->instructions); i++) {
        const IrInstruction* instr = &ir->instructions[i];

        switch (instr->op) {
        case IR_OP_ASSIGN:
            ir_render_value(&sb, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_value(&sb, &instr->rvalue[0]);
            break;
        case IR_OP_RETURN:
            sb_append_cstr(&sb, "return ");
            ir_render_value(&sb, &instr->rvalue[0]);
            break;
        default:
            sb_append_cstr(&sb, "<unknown>");
            break;
        }

        if (i + 1 < array_count(ir->instructions)) {
            sb_append_char(&sb, '\n');
        }
    }

    return sb_to_string(&sb);
}

void ir_save(const Ir* ir, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = ir_render(ir, &arena);

    FILE* file      = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        kill("Failed to open file for writing: %s", path);
    }

    usize written = fwrite(rendered.data, 1, rendered.count, file);
    fclose(file);
    arena_done(&arena);

    if (written != rendered.count) {
        kill("Failed to write IR file: %s", path);
    }
}

void ir_dump(const Ir* ir)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = ir_render(ir, &arena);
    prn("\nIR:\n");
    if (rendered.count > 0) {
        prn(STRINGP, STRINGV(rendered));
    }
    arena_done(&arena);
}
