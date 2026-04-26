//------------------------------------------------------------------------------
// IR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/error/error.h>
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

internal void ir_render_label(StringBuilder* sb, i64 label_id)
{
    sb_format(sb, "L%lld", label_id);
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
    case IR_VALUE_FLOAT:
        {
            string rendered =
                string_format(&temp_arena, "%.17g", value->value.floating);
            bool needs_decimal = true;
            for (usize i = 0; i < rendered.count; ++i) {
                if (rendered.data[i] == '.' || rendered.data[i] == 'e' ||
                    rendered.data[i] == 'E') {
                    needs_decimal = false;
                    break;
                }
            }
            sb_append_string(sb, rendered);
            if (needs_decimal) {
                sb_append_cstr(sb, ".0");
            }
        }
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

internal void ir_render_type_name(StringBuilder* sb,
                                  const Ir*      ir,
                                  const Lexer*   lexer,
                                  u32            type_index)
{
    if (type_index == U32_MAX || type_index >= array_count(ir->types)) {
        sb_append_cstr(sb, "<unknown>");
        return;
    }

    const SemaType* type = &ir->types[type_index];
    switch (type->kind) {
    case STK_Void:
        sb_append_cstr(sb, "void");
        break;
    case STK_UntypedInteger:
        sb_append_cstr(sb, "untyped-integer");
        break;
    case STK_UntypedFloat:
        sb_append_cstr(sb, "untyped-float");
        break;
    case STK_String:
        sb_append_cstr(sb, "string");
        break;
    case STK_Bool:
        sb_append_cstr(sb, "bool");
        break;
    case STK_I8:
        sb_append_cstr(sb, "i8");
        break;
    case STK_I16:
        sb_append_cstr(sb, "i16");
        break;
    case STK_I32:
        sb_append_cstr(sb, "i32");
        break;
    case STK_I64:
        sb_append_cstr(sb, "i64");
        break;
    case STK_U8:
        sb_append_cstr(sb, "u8");
        break;
    case STK_U16:
        sb_append_cstr(sb, "u16");
        break;
    case STK_U32:
        sb_append_cstr(sb, "u32");
        break;
    case STK_U64:
        sb_append_cstr(sb, "u64");
        break;
    case STK_F32:
        sb_append_cstr(sb, "f32");
        break;
    case STK_F64:
        sb_append_cstr(sb, "f64");
        break;
    case STK_Isize:
        sb_append_cstr(sb, "isize");
        break;
    case STK_Usize:
        sb_append_cstr(sb, "usize");
        break;
    case STK_Function:
        sb_append_cstr(sb, "fn(");
        for (u32 i = 0; i < type->param_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ",");
            }
            ir_render_type_name(
                sb,
                ir,
                lexer,
                ir->type_param_types[type->first_param_type + i]);
        }
        if (type->flags & STF_FunctionVarargs) {
            if (type->param_count > 0) {
                sb_append_cstr(sb, ",");
            }
            sb_append_cstr(sb, "...");
        }
        sb_append_cstr(sb, ")->");
        ir_render_type_name(sb, ir, lexer, type->return_type);
        break;
    case STK_Module:
        sb_append_cstr(sb, "module");
        break;
    case STK_Tuple:
        sb_append_cstr(sb, "(");
        for (u32 i = 0; i < type->param_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ",");
            }
            ir_render_type_name(
                sb,
                ir,
                lexer,
                ir->type_param_types[type->first_param_type + i]);
        }
        if (type->param_count == 1) {
            sb_append_cstr(sb, ",");
        }
        sb_append_cstr(sb, ")");
        break;
    case STK_Array:
        sb_format(sb, "[%u]", type->return_type);
        ir_render_type_name(sb, ir, lexer, type->first_param_type);
        break;
    case STK_Slice:
        sb_append_cstr(sb, "[]");
        ir_render_type_name(sb, ir, lexer, type->first_param_type);
        break;
    case STK_Pointer:
        sb_append_char(sb, '^');
        ir_render_type_name(sb, ir, lexer, type->first_param_type);
        break;
    case STK_Plex:
    case STK_Union:
        sb_append_cstr(sb, type->kind == STK_Plex ? "plex{" : "union{");
        for (u32 i = 0; i < type->param_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ",");
            }
            sb_append_string(
                sb,
                lex_symbol(lexer,
                           ir->type_param_symbols[type->first_param_type + i]));
            sb_append_char(sb, ':');
            ir_render_type_name(
                sb,
                ir,
                lexer,
                ir->type_param_types[type->first_param_type + i]);
        }
        sb_append_cstr(sb, "}");
        break;
    case STK_Enum:
        sb_append_cstr(sb, "enum{");
        for (u32 i = 0; i < type->param_count; ++i) {
            if (i > 0) {
                sb_append_cstr(sb, ",");
            }
            sb_append_string(
                sb,
                lex_symbol(lexer,
                           ir->type_param_symbols[type->first_param_type + i]));
            u32 payload_type = ir->type_param_types[type->first_param_type + i];
            if (payload_type != sema_no_type()) {
                sb_append_char(sb, '(');
                ir_render_type_name(sb, ir, lexer, payload_type);
                sb_append_char(sb, ')');
            }
        }
        sb_append_cstr(sb, "}");
        break;
    default:
        sb_append_cstr(sb, "<unknown>");
        break;
    }
}

internal void ir_render_typed_value(StringBuilder* sb,
                                    const Ir*      ir,
                                    const Lexer*   lexer,
                                    const IrValue* value,
                                    u32            type_index)
{
    ir_render_type_name(sb, ir, lexer, type_index);
    sb_append_char(sb, ':');
    if (type_index != U32_MAX && value->kind == IR_VALUE_INTEGER &&
        ir->types[type_index].kind == STK_Bool) {
        sb_append_cstr(sb, value->value.integer != 0 ? "yes" : "no");
        return;
    }
    ir_render_value(sb, ir, lexer, value);
}

internal void ir_render_maybe_typed_value(StringBuilder* sb,
                                          const Ir*      ir,
                                          const Lexer*   lexer,
                                          const IrValue* value)
{
    if (value->type != U32_MAX) {
        ir_render_typed_value(sb, ir, lexer, value, value->type);
        return;
    }

    ir_render_value(sb, ir, lexer, value);
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
        case IR_OP_BLOCK_START:
            sb_append_cstr(&sb, "block");
            break;
        case IR_OP_BLOCK_END:
            sb_append_cstr(&sb, "end");
            break;
        case IR_OP_PARAM:
            sb_append_cstr(&sb, "param ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->lvalue);
            break;
        case IR_OP_LOCAL:
            sb_append_cstr(&sb, "local ");
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_ASSIGN:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_CALL:
            if (instr->lvalue.kind != IR_VALUE_NONE) {
                ir_render_value(&sb, ir, lexer, &instr->lvalue);
                sb_append_cstr(&sb, " = ");
            }
            sb_append_cstr(&sb, "call ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            {
                const IrCallInfo* call =
                    &ir->calls[(u32)instr->rvalue[1].value.integer];
                for (u32 i = 0; i < call->arg_count; ++i) {
                    sb_append_cstr(&sb, i == 0 ? ", " : ", ");
                    IrValue value = ir->call_args[call->first_arg + i].value;
                    value.type    = ir->call_args[call->first_arg + i].type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &value);
                }
            }
            break;
        case IR_OP_CAST:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = cast ");
            ir_render_typed_value(
                &sb, ir, lexer, &instr->rvalue[0], instr->rvalue[0].type);
            break;
        case IR_OP_TUPLE:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = tuple(");
            {
                const IrTupleInfo* tuple =
                    &ir->tuples[(u32)instr->rvalue[0].value.integer];
                for (u32 item = 0; item < tuple->item_count; ++item) {
                    if (item > 0) {
                        sb_append_cstr(&sb, ", ");
                    }
                    IrValue value =
                        ir->tuple_items[tuple->first_item + item].value;
                    value.type = ir->tuple_items[tuple->first_item + item].type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &value);
                }
            }
            sb_append_cstr(&sb, ")");
            break;
        case IR_OP_PLEX:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb,
                           ir->types[instr->lvalue.type].kind == STK_Union
                               ? " = union("
                               : " = plex(");
            {
                const IrTupleInfo* plex =
                    &ir->tuples[(u32)instr->rvalue[0].value.integer];
                for (u32 item = 0; item < plex->item_count; ++item) {
                    if (item > 0) {
                        sb_append_cstr(&sb, ", ");
                    }
                    const IrTupleItem* plex_item =
                        &ir->tuple_items[plex->first_item + item];
                    sb_append_string(&sb, lex_symbol(lexer, plex_item->symbol));
                    sb_append_cstr(&sb, ": ");
                    IrValue value = plex_item->value;
                    value.type    = plex_item->type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &value);
                }
            }
            sb_append_cstr(&sb, ")");
            break;
        case IR_OP_TUPLE_FIELD:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_format(&sb, ".%u", (u32)instr->rvalue[1].value.integer);
            break;
        case IR_OP_ARRAY:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = array[");
            {
                const IrTupleInfo* array =
                    &ir->tuples[(u32)instr->rvalue[0].value.integer];
                for (u32 item = 0; item < array->item_count; ++item) {
                    if (item > 0) {
                        sb_append_cstr(&sb, ", ");
                    }
                    IrValue value =
                        ir->tuple_items[array->first_item + item].value;
                    value.type = ir->tuple_items[array->first_item + item].type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &value);
                }
            }
            sb_append_cstr(&sb, "]");
            break;
        case IR_OP_SLICE:
            {
                const IrSliceInfo* slice =
                    &ir->slices[(u32)instr->rvalue[0].value.integer];
                ir_render_value(&sb, ir, lexer, &instr->lvalue);
                sb_append_cstr(&sb, " = ");
                IrValue target = slice->target;
                target.type    = slice->target_type;
                ir_render_maybe_typed_value(&sb, ir, lexer, &target);
                sb_append_char(&sb, '[');
                if (slice->start.kind != IR_VALUE_NONE) {
                    IrValue start = slice->start;
                    start.type    = slice->start_type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &start);
                }
                sb_append_cstr(&sb, "..");
                if (slice->end.kind != IR_VALUE_NONE) {
                    IrValue end = slice->end;
                    end.type    = slice->end_type;
                    ir_render_maybe_typed_value(&sb, ir, lexer, &end);
                }
                sb_append_char(&sb, ']');
            }
            break;
        case IR_OP_ENUM:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_format(
                &sb,
                " = enum(%lld)",
                ir->type_param_values[ir->types[instr->lvalue.type]
                                          .first_param_type +
                                      (u32)instr->rvalue[0].value.integer]);
            if (instr->rvalue[1].kind != IR_VALUE_NONE) {
                sb_append_cstr(&sb, " ");
                ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[1]);
            }
            break;
        case IR_OP_ENUM_PAYLOAD:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_format(&sb, ".payload(%lld)", instr->rvalue[1].value.integer);
            break;
        case IR_OP_FIELD:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_append_char(&sb, '.');
            sb_append_string(
                &sb, lex_symbol(lexer, (u32)instr->rvalue[1].value.integer));
            break;
        case IR_OP_INDEX:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_append_char(&sb, '[');
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[1]);
            sb_append_char(&sb, ']');
            break;
        case IR_OP_ADDRESS_OF:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ^");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_ADDRESS_OF_INDEX:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ^");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_append_char(&sb, '[');
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[1]);
            sb_append_char(&sb, ']');
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
            ir_render_typed_value(
                &sb, ir, lexer, &instr->rvalue[0], instr->rvalue[0].type);
            break;
        case IR_OP_STRING_FINISH:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = string.finish ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_BRANCH_FALSE:
            sb_append_cstr(&sb, "branch.false ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            sb_append_cstr(&sb, ", ");
            ir_render_label(&sb, instr->rvalue[1].value.integer);
            break;
        case IR_OP_JUMP:
            sb_append_cstr(&sb, "jump ");
            ir_render_label(&sb, instr->rvalue[0].value.integer);
            break;
        case IR_OP_LABEL:
            sb_append_cstr(&sb, "label ");
            ir_render_label(&sb, instr->lvalue.value.integer);
            break;
        case IR_OP_EQUAL:
        case IR_OP_NOT_EQUAL:
        case IR_OP_LESS:
        case IR_OP_LESS_EQUAL:
        case IR_OP_BITWISE_AND:
        case IR_OP_BITWISE_XOR:
        case IR_OP_BITWISE_OR:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            switch (instr->op) {
            case IR_OP_EQUAL:
                sb_append_cstr(&sb, " == ");
                break;
            case IR_OP_NOT_EQUAL:
                sb_append_cstr(&sb, " != ");
                break;
            case IR_OP_LESS:
                sb_append_cstr(&sb, " < ");
                break;
            case IR_OP_LESS_EQUAL:
                sb_append_cstr(&sb, " <= ");
                break;
            case IR_OP_BITWISE_AND:
                sb_append_cstr(&sb, " & ");
                break;
            case IR_OP_BITWISE_XOR:
                sb_append_cstr(&sb, " ^ ");
                break;
            case IR_OP_BITWISE_OR:
                sb_append_cstr(&sb, " | ");
                break;
            default:
                break;
            }
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[1]);
            break;
        case IR_OP_LOGICAL_NOT:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = !");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_NEGATE:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = -");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
            break;
        case IR_OP_ADD:
        case IR_OP_SUBTRACT:
        case IR_OP_MULTIPLY:
        case IR_OP_DIVIDE:
        case IR_OP_MODULO:
            ir_render_value(&sb, ir, lexer, &instr->lvalue);
            sb_append_cstr(&sb, " = ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
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
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[1]);
            break;
        case IR_OP_RETURN:
            sb_append_cstr(&sb, "return ");
            ir_render_maybe_typed_value(&sb, ir, lexer, &instr->rvalue[0]);
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

bool ir_save(const Ir* ir, const Lexer* lexer, cstr path)
{
    Arena arena = {0};
    arena_init(&arena);
    string rendered = ir_render(ir, lexer, &arena);

    FILE* file      = fopen(path, "wb");
    if (!file) {
        arena_done(&arena);
        return error_runtime("Failed to open file for writing: %s", path);
    }

    usize written      = fwrite(rendered.data, 1, rendered.count, file);
    bool  close_failed = fclose(file) != 0;
    arena_done(&arena);

    if (written != rendered.count || close_failed) {
        return error_runtime("Failed to write IR file: %s", path);
    }
    return true;
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
