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

internal void cgen_add_value(CGen* cgen, const IrValue* value);

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

internal cstr cgen_c_integer_type(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return "int";
    }

    switch (sema->types[type_index].kind) {
    case STK_I8:
        return "int8_t";
    case STK_I16:
        return "int16_t";
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

internal cstr cgen_c_type(const Sema* sema, u32 type_index)
{
    if (type_index == sema_no_type()) {
        return "int";
    }

    switch (sema->types[type_index].kind) {
    case STK_String:
        return "string";
    case STK_Bool:
        return "bool";
    case STK_F32:
        return "float";
    case STK_F64:
        return "double";
    default:
        return cgen_c_integer_type(sema, type_index);
    }
}

internal u32 cgen_materialise_type(const CGen* cgen, u32 type_index)
{
    return sema_materialise_type(cgen->sema, type_index);
}

internal cstr cgen_string_helper_suffix(const CGen* cgen, u32 type_index)
{
    type_index = cgen_materialise_type(cgen, type_index);
    if (type_index == sema_no_type()) {
        return "i32";
    }

    switch (cgen->sema->types[type_index].kind) {
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

internal u32 cgen_find_decl_index(const CGen* cgen, u32 symbol_handle)
{
    for (u32 i = 0; i < array_count(cgen->sema->decls); ++i) {
        if (cgen->sema->decls[i].symbol_handle == symbol_handle) {
            return i;
        }
    }

    return sema_no_decl();
}

internal u32 cgen_find_local_type_index(const CGen* cgen, u32 symbol_handle)
{
    if (cgen->current_function_decl_index == sema_no_decl()) {
        return sema_no_type();
    }

    for (u32 i = 0; i < array_count(cgen->sema->locals); ++i) {
        const SemaLocal* local = &cgen->sema->locals[i];
        if (local->owner_decl_index == cgen->current_function_decl_index &&
            local->symbol_handle == symbol_handle) {
            return local->type_index;
        }
    }

    return sema_no_type();
}

internal u32 cgen_lvalue_type(const CGen* cgen, const IrValue* value)
{
    switch (value->kind) {
    case IR_VALUE_SYMBOL:
        {
            u32 decl_index =
                cgen_find_decl_index(cgen, (u32)value->value.integer);
            return decl_index == sema_no_decl()
                       ? sema_no_type()
                       : cgen->sema->decls[decl_index].type_index;
        }
    case IR_VALUE_LOCAL:
        return cgen_find_local_type_index(cgen, (u32)value->value.integer);
    default:
        return sema_no_type();
    }
}

internal void cgen_add_zero_value(CGen* cgen, u32 type_index)
{
    if (type_index == sema_no_type()) {
        cgen_add(cgen, "0");
        return;
    }

    switch (cgen->sema->types[type_index].kind) {
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
    if (value->kind == IR_VALUE_INTEGER && value->value.integer == 0 &&
        !sema_type_is_integer(cgen->sema, type_index)) {
        cgen_add_zero_value(cgen, type_index);
        return;
    }

    cgen_add_value(cgen, value);
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
        kill("Unknown IR value kind: %u", value->kind);
        break;
    }
}

//------------------------------------------------------------------------------
// Emit a C assignment from an IR assignment instruction.

void cgen_add_assign(CGen* cgen, const IrInstruction* instr)
{
    cgen_start_line(cgen);
    ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
               instr->lvalue.kind == IR_VALUE_LOCAL,
           "Expected assignable lvalue");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_typed_value(
        cgen, &instr->rvalue[0], cgen_lvalue_type(cgen, &instr->lvalue));
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
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_add(cgen, "(");
    cgen_add_value(cgen, &instr->rvalue[1]);
    cgen_addn(cgen, ");");
}

//------------------------------------------------------------------------------
// Emit a C cast expression from an IR cast instruction.

void cgen_add_cast(CGen* cgen, const IrInstruction* instr)
{
    u32 target_type = instr->lvalue_type;
    cgen_start_line(cgen);
    if (instr->lvalue.kind == IR_VALUE_VARIABLE) {
        cgen_add(cgen, cgen_c_type(cgen->sema, target_type));
        cgen_add(cgen, " ");
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
               "Expected assignable lvalue");
    }
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = (");
    cgen_add(cgen, cgen_c_type(cgen->sema, target_type));
    cgen_add(cgen, ")");
    cgen_add_value(cgen, &instr->rvalue[0]);
    cgen_addn(cgen, ";");
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
    u32  type_index = instr->rvalue_type[0];
    cstr suffix     = cgen_string_helper_suffix(cgen, type_index);
    ASSERT(suffix != NULL, "Expected interpolatable type suffix");

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
        cgen_add(cgen, "int ");
    } else {
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
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
        ASSERT(instr->lvalue.kind == IR_VALUE_SYMBOL ||
                   instr->lvalue.kind == IR_VALUE_LOCAL,
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
    cgen_add(cgen,
             cgen_c_type(cgen->sema, cgen_lvalue_type(cgen, &instr->lvalue)));
    cgen_add(cgen, " ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_addn(cgen, ";");
}

void cgen_add_local(CGen* cgen, const IrInstruction* instr)
{
    ASSERT(instr->lvalue.kind == IR_VALUE_LOCAL, "Expected local symbol");
    u32 type_index = cgen_lvalue_type(cgen, &instr->lvalue);
    cgen_start_line(cgen);
    cgen_add(cgen, cgen_c_type(cgen->sema, type_index));
    cgen_add(cgen, " ");
    cgen_add_value(cgen, &instr->lvalue);
    cgen_add(cgen, " = ");
    cgen_add_typed_value(cgen, &instr->rvalue[0], type_index);
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
        cgen_add_line(cgen, "void init() {}");
    }

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
            cgen_start_line(cgen);
            cgen->current_function_decl_index =
                cgen_find_decl_index(cgen, (u32)instr->lvalue.value.integer);
            u32 return_type = sema_no_type();
            if (cgen->current_function_decl_index != sema_no_decl()) {
                u32 fn_type =
                    cgen->sema->decls[cgen->current_function_decl_index]
                        .type_index;
                if (fn_type != sema_no_type() &&
                    cgen->sema->types[fn_type].kind == STK_Function) {
                    return_type = cgen->sema->types[fn_type].b;
                }
            }
            cgen_add(cgen, cgen_c_type(cgen->sema, return_type));
            cgen_add(cgen, " ");
            cgen_add_symbol_name(cgen, (u32)instr->lvalue.value.integer);
            cgen_addn(cgen, "() {");
            cgen_indent(cgen);
            break;
        case IR_OP_FN_END:
            cgen->current_function_decl_index = sema_no_decl();
            cgen_dedent(cgen);
            cgen_add_line(cgen, "}");
            break;
        case IR_OP_LOCAL:
            cgen_add_local(cgen, instr);
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

CGen cgen_init(const Ir* ir, const Lexer* lexer, const Sema* sema)
{
    CGen cgen = {.ir                          = ir,
                 .lexer                       = lexer,
                 .sema                        = sema,
                 .current_function_decl_index = sema_no_decl()};
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
