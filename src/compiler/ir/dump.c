//------------------------------------------------------------------------------
// IR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ir/ir.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Render a single IR value into a string builder.

internal void ir_render_string(StringBuilder* sb, string text)
{
    sb_append_char(sb, '"');
    for (usize i = 0; i < text.count; ++i) {
        switch (text.data[i]) {
        case '\n':
            sb_append_cstr(sb, "\\n");
            break;
        case '\r':
            sb_append_cstr(sb, "\\r");
            break;
        case '\t':
            sb_append_cstr(sb, "\\t");
            break;
        case '\\':
            sb_append_cstr(sb, "\\\\");
            break;
        case '"':
            sb_append_cstr(sb, "\\\"");
            break;
        default:
            sb_append_char(sb, (char)text.data[i]);
            break;
        }
    }
    sb_append_char(sb, '"');
}

internal void ir_render_value(StringBuilder* sb,
                              const Ir*      ir,
                              const Lexer*   lexer,
                              const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        sb_format(sb, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_LOCAL:
        sb_append_string(sb, lex_symbol(lexer, (u32)value->value.integer));
        break;
    case IR_VALUE_INTEGER:
        sb_format(sb, "%lld", value->value.integer);
        break;
    case IR_VALUE_SYMBOL:
        sb_append_string(sb, lex_symbol(lexer, (u32)value->value.integer));
        break;
    case IR_VALUE_BUILTIN:
        sb_append_string(sb, lex_symbol(lexer, (u32)value->value.integer));
        break;
    case IR_VALUE_STRING:
        ir_render_string(sb, ir->strings[(u32)value->value.integer]);
        break;
    case IR_VALUE_NONE:
        break;
    default:
        sb_append_cstr(sb, "<unknown>");
        break;
    }
}

//------------------------------------------------------------------------------
// Render IR to its stable textual snapshot form.

string ir_render(const Ir* ir, const Lexer* lexer, Arena* arena)
{
    StringBuilder sb = {0};
    sb_init(&sb, arena);

    for (usize i = 0; i < array_count(ir->instructions); i++) {
        const IrInstruction* instr = &ir->instructions[i];

        switch (instr->op) {
        case IR_OP_GLOBAL:
            sb_append_cstr(&sb, "global ");
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            break;
        case IR_OP_INIT_START:
            sb_append_cstr(&sb, "init");
            break;
        case IR_OP_INIT_END:
            sb_append_cstr(&sb, "end");
            break;
        case IR_OP_FN_START:
            sb_append_cstr(&sb, "fn ");
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            break;
        case IR_OP_FN_END:
            sb_append_cstr(&sb, "end");
            break;
        case IR_OP_LOCAL:
            sb_append_cstr(&sb, "local ");
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_ASSIGN:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_CALL:
            sb_append_cstr(&sb, "call ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_append_cstr(&sb, ", ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[1]);
            break;
        case IR_OP_CAST:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = cast ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_format(&sb, ", type=%u", (u32)instr->rvalue[1].value.integer);
            break;
        case IR_OP_STRING_RESET:
            sb_append_cstr(&sb, "string.reset");
            break;
        case IR_OP_STRING_START:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = string.start");
            break;
        case IR_OP_STRING_APPEND:
            sb_append_cstr(&sb, "string.append ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_format(&sb, ", type=%u", (u32)instr->rvalue[1].value.integer);
            break;
        case IR_OP_STRING_FINISH:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = string.finish ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_NEGATE:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = -");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_ADD:
        case IR_OP_SUBTRACT:
        case IR_OP_MULTIPLY:
        case IR_OP_DIVIDE:
        case IR_OP_MODULO:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
            switch (instr->op) {
            case IR_OP_ADD:
                sb_append_cstr(&sb, " + ");
                break;
            case IR_OP_SUBTRACT:
                sb_append_cstr(&sb, " - ");
                break;
            case IR_OP_MULTIPLY:
                sb_append_cstr(&sb, " * ");
                break;
            case IR_OP_DIVIDE:
                sb_append_cstr(&sb, " / ");
                break;
            case IR_OP_MODULO:
                sb_append_cstr(&sb, " % ");
                break;
            default:
                break;
            }
            ir_render_value(&sb, ir, lexer, &instr->rvalue[1]);
            break;
        case IR_OP_RETURN:
            sb_append_cstr(&sb, "return ");
            ir_render_value(&sb, ir, lexer, &instr->rvalue[0]);
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

//------------------------------------------------------------------------------
// Save rendered IR to a file.

void ir_save(const Ir* ir, const Lexer* lexer, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = ir_render(ir, lexer, &arena);

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

//------------------------------------------------------------------------------
// Print rendered IR for debugging.

void ir_dump(const Ir* ir, const Lexer* lexer)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = ir_render(ir, lexer, &arena);
    prn("\nIR:\n");
    if (rendered.count > 0) {
        prn(STRINGP, STRINGV(rendered));
    }
    arena_done(&arena);
}
