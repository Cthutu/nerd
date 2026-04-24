//------------------------------------------------------------------------------
// C Generation Implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/cgen/cgen.h>
#include <compiler/error/error.h>
#include <stdio.h>

static const char g_cgen_prelude[] = {
#embed "../../../data/prelude.c"
    , 0};

static const char g_cgen_epilogue[] = {
#embed "../../../data/epilogue.c"
    , 0};

internal void cgen_add_value(CGen* cgen, const IrValue* value);
internal void cgen_add_float_literal(CGen* cgen, f64 value, u32 type_index);

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
// Render a C symbol name for a built-in runtime function.

void cgen_add_builtin_name(CGen* cgen, u32 symbol_handle)
{
    arena_format(
        &cgen->arena, STRINGP, STRINGV(lex_symbol(cgen->lexer, symbol_handle)));
}

//------------------------------------------------------------------------------
// Render one decoded string literal as a C string expression.

void cgen_add_c_string_literal(CGen* cgen, string text)
{
    cgen_add(cgen, "\"");
    for (usize i = 0; i < text.count; ++i) {
        switch (text.data[i]) {
        case '\n':
            cgen_add(cgen, "\\n");
            break;
        case '\r':
            cgen_add(cgen, "\\r");
            break;
        case '\t':
            cgen_add(cgen, "\\t");
            break;
        case '\\':
            cgen_add(cgen, "\\\\");
            break;
        case '"':
            cgen_add(cgen, "\\\"");
            break;
        default:
            arena_format(&cgen->arena, "%c", text.data[i]);
            break;
        }
    }
    cgen_add(cgen, "\"");
}

internal cstr cgen_c_integer_type(const Ir* ir, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return "int";
    }

    switch (ir->types[type_index].kind) {
    case STK_I8:
        return "int8_t";
    case STK_I16:
        return "int16_t";
    case STK_UntypedInteger:
    case STK_I32:
        return "int";
    case STK_I64:
        return "int64_t";
    case STK_U8:
        return "uint8_t";
    case STK_U16:
        return "uint16_t";
    case STK_U32:
        return "uint32_t";
    case STK_U64:
        return "uint64_t";
    case STK_Isize:
        return "intptr_t";
    case STK_Usize:
        return "uintptr_t";
    default:
        return "int";
    }
}

internal cstr cgen_c_type(const Ir* ir, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return "int";
    }

    switch (ir->types[type_index].kind) {
    case STK_Tuple:
        {
            static char names[8][32];
            static u32  next = 0;
            char*       name = names[next++ % 8];
            snprintf(name, 32, "tuple%u", type_index);
            return name;
        }
    case STK_String:
        return "string";
    case STK_Bool:
        return "bool";
    case STK_UntypedFloat:
        return "double";
    case STK_F32:
        return "float";
    case STK_F64:
        return "double";
    default:
        return cgen_c_integer_type(ir, type_index);
    }
}

internal void
cgen_add_decl_type_and_name(CGen* cgen, u32 type_index, const IrValue* value)
{
    if (type_index != sema_no_type() &&
        cgen->ir->types[type_index].kind == STK_Function) {
        const SemaType* fn_type = &cgen->ir->types[type_index];
        cgen_add(cgen, cgen_c_type(cgen->ir, fn_type->return_type));
        cgen_add(cgen, " (*");
        cgen_add_value(cgen, value);
        cgen_add(cgen, ")(");
        for (u32 i = 0; i < fn_type->param_count; ++i) {
            if (i > 0) {
                cgen_add(cgen, ", ");
            }
            cgen_add(
                cgen,
                cgen_c_type(
                    cgen->ir,
                    cgen->ir->type_param_types[fn_type->first_param_type + i]));
        }
        cgen_add(cgen, ")");
        return;
    }

    cgen_add(cgen, cgen_c_type(cgen->ir, type_index));
    cgen_add(cgen, " ");
    cgen_add_value(cgen, value);
}

internal u32 cgen_materialise_type(const CGen* cgen, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return type_index;
    }

    if (cgen->ir->types[type_index].kind != STK_UntypedInteger &&
        cgen->ir->types[type_index].kind != STK_UntypedFloat) {
        return type_index;
    }

    for (u32 i = 0; i < array_count(cgen->ir->types); ++i) {
        if ((cgen->ir->types[type_index].kind == STK_UntypedInteger &&
             cgen->ir->types[i].kind == STK_I32) ||
            (cgen->ir->types[type_index].kind == STK_UntypedFloat &&
             cgen->ir->types[i].kind == STK_F64)) {
            return i;
        }
    }

    return type_index;
}

internal cstr cgen_string_helper_suffix(const CGen* cgen, u32 type_index)
{
    type_index = cgen_materialise_type(cgen, type_index);
    if (type_index == sema_no_type()) {
        return "i32";
    }

    switch (cgen->ir->types[type_index].kind) {
    case STK_UntypedInteger:
    case STK_I32:
        return "i32";
    case STK_String:
        return "string";
    case STK_Bool:
        return "bool";
    case STK_I8:
        return "i8";
    case STK_I16:
        return "i16";
    case STK_I64:
        return "i64";
    case STK_U8:
        return "u8";
    case STK_U16:
        return "u16";
    case STK_U32:
        return "u32";
    case STK_U64:
        return "u64";
    case STK_F32:
        return "f32";
    case STK_F64:
        return "f64";
    case STK_Isize:
        return "isize";
    case STK_Usize:
        return "usize";
    default:
        return NULL;
    }
}

internal bool cgen_type_is_integer(const CGen* cgen, u32 type_index)
{
    type_index = cgen_materialise_type(cgen, type_index);
    if (type_index == sema_no_type()) {
        return false;
    }

    switch (cgen->ir->types[type_index].kind) {
    case STK_UntypedInteger:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Isize:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}

internal bool cgen_type_is_string(const CGen* cgen, u32 type_index)
{
    type_index = cgen_materialise_type(cgen, type_index);
    return type_index != sema_no_type() &&
           cgen->ir->types[type_index].kind == STK_String;
}

internal void cgen_add_zero_value(CGen* cgen, u32 type_index)
{
    if (type_index == sema_no_type()) {
        cgen_add(cgen, "0");
        return;
    }

    switch (cgen->ir->types[type_index].kind) {
    case STK_Tuple:
        cgen_add(cgen, "(");
        cgen_add(cgen, cgen_c_type(cgen->ir, type_index));
        cgen_add(cgen, "){0}");
        break;
    case STK_String:
        cgen_add(cgen, "(string){0}");
        break;
    case STK_Bool:
        cgen_add(cgen, "false");
        break;
    case STK_F32:
        cgen_add(cgen, "0.0f");
        break;
    case STK_F64:
        cgen_add(cgen, "0.0");
        break;
    default:
        cgen_add(cgen, "0");
        break;
    }
}

internal void
cgen_add_typed_value(CGen* cgen, const IrValue* value, u32 type_index)
{
    if (value->kind == IR_VALUE_INTEGER && type_index != sema_no_type() &&
        cgen->ir->types[type_index].kind == STK_Bool) {
        cgen_add(cgen, value->value.integer != 0 ? "true" : "false");
        return;
    }

    if (value->kind == IR_VALUE_INTEGER && value->value.integer == 0 &&
        !cgen_type_is_integer(cgen, type_index)) {
        cgen_add_zero_value(cgen, type_index);
        return;
    }

    if (value->kind == IR_VALUE_FLOAT) {
        cgen_add_float_literal(cgen, value->value.floating, type_index);
        return;
    }

    cgen_add_value(cgen, value);
}

internal void cgen_add_float_literal(CGen* cgen, f64 value, u32 type_index)
{
    char rendered[64] = {0};
    int  len          = snprintf(rendered, sizeof(rendered), "%.17g", value);
    ASSERT(len > 0 && (usize)len < sizeof(rendered),
           "Float literal rendering overflow");
    bool needs_decimal = true;
    for (int i = 0; i < len; ++i) {
        if (rendered[i] == '.' || rendered[i] == 'e' || rendered[i] == 'E') {
            needs_decimal = false;
            break;
        }
    }

    cgen_add(cgen, rendered);
    if (needs_decimal) {
        cgen_add(cgen, ".0");
    }
    if (type_index != sema_no_type() &&
        cgen->ir->types[type_index].kind == STK_F32) {
        cgen_add(cgen, "f");
    }
}

//------------------------------------------------------------------------------
// Render an IR value into C syntax.

void cgen_add_value(CGen* cgen, const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        arena_format(&cgen->arena, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_LOCAL:
        cgen_add_symbol_name(cgen, (u32)value->value.integer);
        break;
    case IR_VALUE_INTEGER:
        arena_format(&cgen->arena, "%lld", value->value.integer);
        break;
    case IR_VALUE_FLOAT:
        cgen_add_float_literal(cgen, value->value.floating, value->type);
        break;
    case IR_VALUE_SYMBOL:
        cgen_add_symbol_name(cgen, (u32)value->value.integer);
        break;
    case IR_VALUE_BUILTIN:
        cgen_add_builtin_name(cgen, (u32)value->value.integer);
        break;
    case IR_VALUE_STRING:
        cgen_add(cgen, "(string){.data = (u8*)");
        cgen_add_c_string_literal(cgen,
                                  cgen->ir->strings[(u32)value->value.integer]);
        arena_format(&cgen->arena,
                     ", .count = %zu}",
                     cgen->ir->strings[(u32)value->value.integer].count);
        break;
    default:
        error_ice("Unknown IR value kind: %u", value->kind);
    }
}

//------------------------------------------------------------------------------
// Emit a C assignment from an IR assignment instruction.

void cgen_add_assign(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
               instr->lvalue.kind == IR_VALUE_LOCAL ||
               instr->lvalue.kind == IR_VALUE_VARIABLE,
           "Expected assignable lvalue");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_typed_value(cgen, &instr->rvalue[0], instr->lvalue.type);
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
// Emit a C call from an IR call instruction.

void cgen_add_call(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
        cgen_add(cgen, " = ");
    } else if (instr->lvalue.kind != IR_VALUE_NONE) {
        cgen_add_value(cgen, &instr->lvalue);
        cgen_add(cgen, " = ");
    }
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, "(");
    const IrCallInfo* call =
        &cgen->ir->calls[(u32)instr->rvalue[1].value.integer];
    for (u32 i = 0; i < call->arg_count; ++i) {
        if (i > 0) {
            cgen_add(cgen, ", ");
        }
        cgen_add_value(cgen, &cgen->ir->call_args[call->first_arg + i].value);
    }
    cgen_addn(cgen, ");");
}

//------------------------------------------------------------------------------
// Emit a C cast expression from an IR cast instruction.

void cgen_add_cast(CGen* cgen, const IrInstruction* instr)
{
    u32 target_type = instr->lvalue.type;
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add_decl_type_and_name(cgen, target_type, &instr->lvalue);
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
               "Expected assignable lvalue");
    }
    if (instr->lvalue.kind != IR_VALUE_VARIABLE) {
        cgen_add_value(cgen, &instr->lvalue);
    }
    cgen_add(cgen, " = (");
    cgen_add(cgen, cgen_c_type(cgen->ir, target_type));
    cgen_add(cgen, ")");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
}

void cgen_add_tuple(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
    cgen_add(cgen, " = (");
    cgen_add(cgen, cgen_c_type(cgen->ir, instr->lvalue.type));
    cgen_add(cgen, "){");
    const IrTupleInfo* tuple =
        &cgen->ir->tuples[(u32)instr->rvalue[0].value.integer];
    for (u32 i = 0; i < tuple->item_count; ++i) {
        if (i > 0) {
            cgen_add(cgen, ", ");
        }
        arena_format(&cgen->arena, "._%u = ", i);
        const IrTupleItem* item = &cgen->ir->tuple_items[tuple->first_item + i];
        cgen_add_typed_value(cgen, &item->value, item->type);
    }
    cgen_addn(cgen, "};");
}

void cgen_add_tuple_field(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_value(cgen, &instr->rvalue[0]);
    arena_format(&cgen->arena, "._%u;", (u32)instr->rvalue[1].value.integer);
    cgen_addn(cgen, "");
}

void cgen_add_string_reset(CGen* cgen)
{
    cgen_add_line(cgen, "string_builder_reset();");
}

void cgen_add_string_start(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    ASSERT(instr->lvalue.kind == IR_VALUE_VARIABLE,
           "Expected temporary lvalue for string start");
    cgen_add(cgen, "size_t ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = string_builder_mark();");
    cgen_addn(cgen, "");
}

void cgen_add_string_append(CGen* cgen, const IrInstruction* instr)
{
    u32  type_index = instr->rvalue[0].type;
    cstr suffix     = cgen_string_helper_suffix(cgen, type_index);
    if (suffix == NULL) {
        error_ice("Expected interpolatable type suffix for type index %u",
                  type_index);
    }

    cgen_start_line(cgen);
    cgen_add(cgen, "string_builder_append_string(to_string$");
    cgen_add(cgen, suffix);
    cgen_add(cgen, "(");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, "));");
    cgen_addn(cgen, "");
}

void cgen_add_string_finish(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    ASSERT(instr->lvalue.kind == IR_VALUE_VARIABLE,
           "Expected temporary lvalue for string finish");
    cgen_add(cgen, "string ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = string_builder_finish(");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, ");");
    cgen_addn(cgen, "");
}

//------------------------------------------------------------------------------
// Emit a C unary expression from an IR unary instruction.

void cgen_add_unary(CGen* cgen, const IrInstruction* instr, cstr op)
{
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
               "Expected assignable lvalue");
    }
    if (instr->lvalue.kind != IR_VALUE_VARIABLE) {
        cgen_add_value(cgen, &instr->lvalue);
    }
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
        cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
               "Expected assignable lvalue");
    }
    if (instr->lvalue.kind != IR_VALUE_VARIABLE) {
        cgen_add_value(cgen, &instr->lvalue);
    }
    cgen_add(cgen, " = ");
    bool string_equality =
        (instr->op == IR_OP_EQUAL || instr->op == IR_OP_NOT_EQUAL) &&
        cgen_type_is_string(cgen, instr->rvalue[0].type) &&
        cgen_type_is_string(cgen, instr->rvalue[1].type);
    if (string_equality) {
        if (instr->op == IR_OP_NOT_EQUAL) {
            cgen_add(cgen, "!");
        }
        cgen_add(cgen, "string_eq(");
        cgen_add_value(cgen, &instr->rvalue[0]);
        cgen_add(cgen, ", ");
        cgen_add_value(cgen, &instr->rvalue[1]);
        cgen_addn(cgen, ");");
        return;
    }
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
    cgen_add_decl_type_and_name(cgen, instr->lvalue.type, &instr->lvalue);
    cgen_addn(cgen, ";");
}

internal void cgen_add_tuple_type_decls(CGen* cgen)
{
    for (u32 i = 0; i < array_count(cgen->ir->types); ++i) {
        const SemaType* type = &cgen->ir->types[i];
        if (type->kind != STK_Tuple) {
            continue;
        }
        cgen_start_line(cgen);
        arena_format(
            &cgen->arena, "typedef struct %s {", cgen_c_type(cgen->ir, i));
        cgen_addn(cgen, "");
        cgen_indent(cgen);
        for (u32 field = 0; field < type->param_count; ++field) {
            cgen_start_line(cgen);
            cgen_add(
                cgen,
                cgen_c_type(cgen->ir,
                            cgen->ir->type_param_types[type->first_param_type +
                                                       field]));
            arena_format(&cgen->arena, " _%u;", field);
            cgen_addn(cgen, "");
        }
        cgen_dedent(cgen);
        cgen_start_line(cgen);
        arena_format(&cgen->arena, "} %s;", cgen_c_type(cgen->ir, i));
        cgen_addn(cgen, "");
    }
}

void cgen_add_local(CGen* cgen, const IrInstruction* instr)
{
    ASSERT(instr->lvalue.kind == IR_VALUE_LOCAL ||
               instr->lvalue.kind == IR_VALUE_VARIABLE,
           "Expected local or temporary symbol");
    u32 type_index = instr->lvalue.type;
    cgen_start_line(cgen);
    cgen_add_decl_type_and_name(cgen, type_index, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_typed_value(cgen, &instr->rvalue[0], type_index);
    cgen_addn(cgen, ";");
}

void cgen_add_branch_false(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    cgen_add(cgen, "if (!");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, ") goto ");
    arena_format(&cgen->arena, "L%lld;", instr->rvalue[1].value.integer);
    cgen_addn(cgen, "");
}

void cgen_add_jump(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    arena_format(&cgen->arena, "goto L%lld;", instr->rvalue[0].value.integer);
    cgen_addn(cgen, "");
}

void cgen_add_label(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    arena_format(&cgen->arena, "L%lld: ;", instr->lvalue.value.integer);
    cgen_addn(cgen, "");
}

//------------------------------------------------------------------------------
// C generation

void cgen_generate(CGen* cgen, const Ir* ir)
{
    bool has_init_section = false;
    u32  function_index   = 0;
    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        if (ir->instructions[i].op == IR_OP_INIT_START) {
            has_init_section = true;
            break;
        }
    }

    if (!has_init_section) {
        cgen_add_line(cgen, "void init() {}");
    }

    cgen_add_tuple_type_decls(cgen);

    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        const IrInstruction* instr = &ir->instructions[i];
        switch (instr->op) {
        case IR_OP_GLOBAL:
            cgen_add_global(cgen, instr);
            break;
        case IR_OP_INIT_START:
            cgen_add_line(cgen, "void init() {");
            cgen_indent(cgen);
            break;
        case IR_OP_INIT_END:
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_FN_START:
            ASSERT(function_index < array_count(ir->functions),
                   "Expected function record for fn.start");
            const IrFunction* function = &ir->functions[function_index++];
            cgen_start_line(cgen);
            u32 return_type = sema_no_type();
            u32 fn_type     = instr->lvalue.type;
            if (fn_type != sema_no_type() &&
                cgen->ir->types[fn_type].kind == STK_Function) {
                return_type = cgen->ir->types[fn_type].return_type;
            }
            cgen_add(cgen, cgen_c_type(cgen->ir, return_type));
            cgen_add(cgen, " ");
            cgen_add_symbol_name(cgen, (u32)instr->lvalue.value.integer);
            cgen_add(cgen, "(");
            for (u32 i = 0; i < function->param_count; ++i) {
                if (i > 0) {
                    cgen_add(cgen, ", ");
                }
                const IrLocal* param =
                    &cgen->ir->locals[function->first_local + i];
                ASSERT(param->is_param,
                       "Expected function params first in local table");
                cgen_add_decl_type_and_name(
                    cgen,
                    param->type,
                    &(IrValue){.kind          = IR_VALUE_LOCAL,
                               .type          = param->type,
                               .value.integer = param->symbol});
            }
            cgen_addn(cgen, ") {");
            cgen_indent(cgen);
            break;
        case IR_OP_FN_END:
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_BLOCK_START:
            cgen_add_line(cgen, "{");
            cgen_indent(cgen);
            break;
        case IR_OP_BLOCK_END:
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_LOCAL:
            cgen_add_local(cgen, instr);
            break;
        case IR_OP_PARAM:
            break;
        case IR_OP_ASSIGN:
            cgen_add_assign(cgen, instr);
            break;
        case IR_OP_CALL:
            cgen_add_call(cgen, instr);
            break;
        case IR_OP_CAST:
            cgen_add_cast(cgen, instr);
            break;
        case IR_OP_TUPLE:
            cgen_add_tuple(cgen, instr);
            break;
        case IR_OP_TUPLE_FIELD:
            cgen_add_tuple_field(cgen, instr);
            break;
        case IR_OP_STRING_RESET:
            cgen_add_string_reset(cgen);
            break;
        case IR_OP_STRING_START:
            cgen_add_string_start(cgen, instr);
            break;
        case IR_OP_STRING_APPEND:
            cgen_add_string_append(cgen, instr);
            break;
        case IR_OP_STRING_FINISH:
            cgen_add_string_finish(cgen, instr);
            break;
        case IR_OP_BRANCH_FALSE:
            cgen_add_branch_false(cgen, instr);
            break;
        case IR_OP_JUMP:
            cgen_add_jump(cgen, instr);
            break;
        case IR_OP_LABEL:
            cgen_add_label(cgen, instr);
            break;
        case IR_OP_EQUAL:
            cgen_add_binary(cgen, instr, " == ");
            break;
        case IR_OP_NOT_EQUAL:
            cgen_add_binary(cgen, instr, " != ");
            break;
        case IR_OP_LESS:
            cgen_add_binary(cgen, instr, " < ");
            break;
        case IR_OP_LESS_EQUAL:
            cgen_add_binary(cgen, instr, " <= ");
            break;
        case IR_OP_BITWISE_AND:
            cgen_add_binary(cgen, instr, " & ");
            break;
        case IR_OP_BITWISE_XOR:
            cgen_add_binary(cgen, instr, " ^ ");
            break;
        case IR_OP_BITWISE_OR:
            cgen_add_binary(cgen, instr, " | ");
            break;
        case IR_OP_LOGICAL_NOT:
            cgen_add_unary(cgen, instr, "!");
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
            error_ice("Unknown IR operation: %u", instr->op);
        }
    }
}

//------------------------------------------------------------------------------

// Build the generated C buffer from IR.

CGen cgen_init(const Ir* ir, const Lexer* lexer, const Sema* sema)
{
    UNUSED(sema);
    CGen cgen = {.ir = ir, .lexer = lexer};
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
