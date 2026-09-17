//------------------------------------------------------------------------------
// HIR to C: preserve source evaluation order using explicit temporaries.
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
#include <compiler/build/back/llvm_runtime.h>
#include <compiler/cgen/cgen.h>
#include <compiler/error/error.h>
#include <stdio.h>

static const unsigned char cgen_runtime[] = {
#embed "../../../data/nrt.c"
};

static const unsigned char cgen_helpers[] = {
#embed "../../../data/ncg.c"
};

typedef struct {
    u32    module, type;
    string key;
    u8     state;
} CType;
typedef struct {
    u32  symbol, id, type, cleanup;
    cstr result;
    bool loop;
} CTarget;
typedef struct {
    bool block;
    u32  index;
} CCleanup;
typedef struct {
    cstr text;
    u32  type;
    bool bitfield;
} CValue;

typedef struct {
    u32    module, local;
    CValue value;
} COverride;
typedef struct {
    const ProgramInfo* program;
    Arena              arena, out_arena, body_arena, decl_arena;
    StringBuilder      out, body, declarations;
    Array(CType) types;
    Array(u32) offsets;
    Array(u32) type_map;
    Array(CTarget) targets;
    Array(CCleanup) cleanups;
    Array(bool) locals;
    Array(u32) local_types;
    Array(COverride) overrides;
    string macro_path;
    cstr   interpolation_arena;
    u32    macro_line;
    bool   global_init;
    bool   returning;
    u32    constant_depth, expected_type;
    u32    module, next, return_type, va_local;
    bool   failed;
} CGen;

#define CF(...) cgen_format(c, __VA_ARGS__)
#define OUT(...) sb_format(&c->body, __VA_ARGS__)

internal cstr cgen_format(CGen* c, cstr format, ...)
{
    va_list args;
    va_start(args, format);
    va_list count;
    va_copy(count, args);
    int n = vsnprintf(NULL, 0, format, count);
    va_end(count);
    char* text = arena_alloc(&c->arena, (usize)n + 1);
    vsnprintf(text, (usize)n + 1, format, args);
    va_end(args);
    return text;
}
internal const FrontEndState* cgen_front(CGen* c)
{
    return &c->program->modules[c->module].front_end;
}
internal const Sema*     cgen_sema(CGen* c) { return &cgen_front(c)->sema; }
internal const Hir*      cgen_hir(CGen* c) { return &cgen_front(c)->hir; }
internal const SemaType* cgen_type(CGen* c, u32 t)
{
    static const SemaType empty = {.kind = STK_Void};
    return t < array_count(cgen_sema(c)->types) ? &cgen_sema(c)->types[t]
                                                : &empty;
}
internal SemaTypeKind cgen_kind(CGen* c, u32 t)
{
    return cgen_type(c, t)->kind;
}
internal u32 cgen_builtin(CGen* c, SemaTypeKind kind)
{
    for (u32 i = 0; i < array_count(cgen_sema(c)->types); ++i) {
        if (cgen_kind(c, i) == kind) {
            return i;
        }
    }
    return U32_MAX;
}
internal bool cgen_void(CGen* c, u32 t)
{
    return cgen_kind(c, t) == STK_Void || cgen_kind(c, t) == STK_Never;
}
internal cstr cgen_symbol(CGen* c, u32 sym)
{
    string s0 = sym == U32_MAX ? s("") : lex_symbol(&cgen_front(c)->lexer, sym);
    return CF("%.*s", (int)s0.count, s0.data);
}
internal cstr cgen_quote(CGen* c, string value)
{
    Arena temp = {0};
    arena_init(&temp);
    StringBuilder b = {0};
    sb_init(&b, &temp);
    sb_append_char(&b, '"');
    for (usize i = 0; i < value.count; ++i) {
        sb_format(&b, "\\%03o", value.data[i]);
    }
    sb_append_cstr(&b, "\"");
    sb_append_null(&b);
    cstr result = CF("%s", sb_to_string(&b).data);
    arena_done(&temp);
    return result;
}
internal void cgen_error(CGen* c, cstr what, u32 index)
{
    if (!c->failed) {
        error_runtime("C generation: %s (module %u, HIR index %u)",
                      what,
                      c->module,
                      index);
    }
    c->failed = true;
}
internal u32 cgen_canonical(CGen* c, u32 t)
{
    if (t >= array_count(cgen_sema(c)->types)) {
        return U32_MAX;
    }
    u32 slot = c->offsets[c->module] + t;
    if (c->type_map[slot] != U32_MAX) {
        return c->type_map[slot];
    }
    const SemaType* st   = cgen_type(c, t);
    Arena           temp = {0};
    arena_init(&temp);
    StringBuilder key = {0};
    sb_init(&key, &temp);
    sb_format(&key,
              "%u:%u:%u:",
              st->kind,
              st->flags,
              st->kind == STK_Array ? st->return_type : st->param_count);
    switch (st->kind) {
    case STK_Array:
    case STK_Atomic:
        sb_format(&key, "%u", cgen_canonical(c, st->first_param_type));
        break;
    case STK_Function:
    case STK_Tuple:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
        for (u32 i = 0; i < st->param_count; ++i) {
            u32 p = st->first_param_type + i;
            sb_format(&key,
                      "%u/",
                      cgen_canonical(c, cgen_sema(c)->type_param_types[p]));
            if (st->kind == STK_Plex || st->kind == STK_Union ||
                st->kind == STK_Enum) {
                sb_format(&key,
                          "%s/%lld/%u/%u/",
                          cgen_symbol(c, cgen_sema(c)->type_param_symbols[p]),
                          (long long)cgen_sema(c)->type_param_values[p],
                          cgen_sema(c)->type_param_bit_widths[p],
                          cgen_sema(c)->type_param_bit_offsets[p]);
                if (cgen_sema(c)->type_param_bit_widths[p]) {
                    sb_format(
                        &key,
                        "storage%u/%u/",
                        cgen_canonical(
                            c, cgen_sema(c)->type_param_bit_storage_types[p]),
                        cgen_sema(c)->type_param_bit_starts[p]);
                }
            }
        }
        if (st->kind == STK_Function) {
            sb_format(&key, "r%u", cgen_canonical(c, st->return_type));
        }
        break;
    default:
        break;
    }
    for (u32 i = 0; i < st->plex_use_count; ++i) {
        const SemaPlexUse* use =
            &cgen_sema(c)->plex_uses[st->first_plex_use + i];
        sb_format(&key,
                  "use%u/%u/%u/",
                  use->first_field,
                  use->field_count,
                  cgen_canonical(c, use->type_index));
    }
    string raw = sb_to_string(&key);
    string k   = s(CF("%.*s", (int)raw.count, raw.data));
    arena_done(&temp);
    for (u32 i = 0; i < array_count(c->types); ++i) {
        if (string_eq(k, c->types[i].key)) {
            return c->type_map[slot] = i;
        }
    }
    u32 id = array_count(c->types);
    array_push(c->types, ((CType){c->module, t, k, 0}));
    c->type_map[slot] = id;
    return id;
}
internal cstr cgen_ctype(CGen* c, u32 t)
{
    switch (cgen_kind(c, t)) {
    case STK_Void:
    case STK_Never:
        return "void";
    case STK_UntypedInteger:
    case STK_I64:
        return "int64_t";
    case STK_UntypedFloat:
    case STK_F64:
        return "double";
    case STK_Bool:
        return "bool";
    case STK_I8:
        return "int8_t";
    case STK_I16:
        return "int16_t";
    case STK_I32:
        return "int32_t";
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
    case STK_F32:
        return "float";
    case STK_String:
        return "NerdString";
    case STK_Arena:
        return "NrtArena";
    case STK_VaList:
        return "NrtVaList*";
    case STK_Pointer:
    case STK_Box:
    case STK_Nil:
        return "void*";
    case STK_DynamicArray:
        return "NcgArray*";
    case STK_Atomic:
        return cgen_ctype(c, cgen_type(c, t)->first_param_type);
    case STK_Module:
        return "void*";
    default:
        return CF("ncg_t%u", cgen_canonical(c, t));
    }
}
internal u32 cgen_field_type(CGen* c, u32 t, u32 f)
{
    return cgen_sema(c)
        ->type_param_types[cgen_type(c, t)->first_param_type + f];
}
internal u32 cgen_field_index(CGen* c, u32 t, u32 symbol)
{
    const SemaType* st = cgen_type(c, t);
    for (u32 i = 0; i < st->param_count; ++i) {
        if (cgen_sema(c)->type_param_symbols[st->first_param_type + i] ==
            symbol) {
            return i;
        }
    }
    return U32_MAX;
}
internal cstr cgen_zero(CGen* c, u32 t)
{
    if (cgen_void(c, t)) {
        return "0";
    }
    if (cgen_kind(c, t) == STK_Enum) {
        i64 tag =
            cgen_type(c, t)->param_count
                ? cgen_sema(c)
                      ->type_param_values[cgen_type(c, t)->first_param_type]
                : 0;
        return CF("(%s){.tag=%lld}", cgen_ctype(c, t), (long long)tag);
    }
    return CF("(%s){0}", cgen_ctype(c, t));
}
internal CValue cgen_temp(CGen* c, u32 t, cstr expr)
{
    if (cgen_void(c, t)) {
        if (expr && *expr) {
            OUT("%s;\n", expr);
        }
        return (CValue){"0", t, false};
    }
    cstr name = CF("ncg_v%u", c->next++);
    OUT("%s %s = %s;\n", cgen_ctype(c, t), name, expr ? expr : cgen_zero(c, t));
    return (CValue){name, t, false};
}
internal bool cgen_bitfield(CGen* c, u32 type, u32 field)
{
    const SemaType* t = cgen_type(c, type);
    return t->kind == STK_Plex && field < t->param_count &&
           cgen_sema(c)->type_param_bit_widths[t->first_param_type + field] !=
               0;
}
internal void cgen_record_fields(CGen* c, u32 type, u32 first, u32 depth)
{
    if (depth > 64) {
        cgen_error(c, "recursive embedded record", type);
        return;
    }
    const SemaType* t   = cgen_type(c, type);
    bool            any = false;
    for (u32 i = 0; i < t->param_count;) {
        bool embedded = false;
        for (u32 j = 0; j < t->plex_use_count; ++j) {
            const SemaPlexUse* use =
                &cgen_sema(c)->plex_uses[t->first_plex_use + j];
            if (use->first_field == i && use->field_count) {
                sb_append_cstr(&c->out, "struct {\n");
                cgen_record_fields(c, use->type_index, first + i, depth + 1);
                sb_append_cstr(&c->out, "};\n");
                i += use->field_count;
                embedded = true;
                any      = true;
                break;
            }
        }
        if (embedded) {
            continue;
        }
        u32 p = t->first_param_type + i;
        if (cgen_bitfield(c, type, i)) {
            sb_append_cstr(&c->out, "struct {\n");
            u32 storage = cgen_sema(c)->type_param_bit_storage_types[p];
            u32 bits    = 0;
            do {
                p          = t->first_param_type + i;
                u32 offset = cgen_sema(c)->type_param_bit_offsets[p],
                    width  = cgen_sema(c)->type_param_bit_widths[p];
                if (offset > bits) {
                    sb_format(&c->out,
                              "%s : %u;\n",
                              cgen_ctype(c, storage),
                              offset - bits);
                }
                sb_format(&c->out,
                          "%s f%u : %u;\n",
                          cgen_ctype(c, storage),
                          first + i,
                          width);
                bits = offset + width;
                ++i;
            } while (
                i < t->param_count && cgen_bitfield(c, type, i) &&
                !cgen_sema(c)->type_param_bit_starts[t->first_param_type + i]);
            sb_append_cstr(&c->out, "};\n");
            any = true;
            continue;
        }
        u32 ft = cgen_field_type(c, type, i);
        if (!cgen_void(c, ft)) {
            sb_format(&c->out, "%s f%u;\n", cgen_ctype(c, ft), first + i);
            any = true;
        }
        ++i;
    }
    if (!any) {
        sb_append_cstr(&c->out, "unsigned char empty;\n");
    }
}

internal void cgen_define_type(CGen* c, u32 id)
{
    if (id == U32_MAX || c->types[id].state == 2) {
        return;
    }
    if (c->types[id].state == 1) {
        cgen_error(c, "cyclic by-value type", id);
        return;
    }
    c->types[id].state = 1;
    u32 old            = c->module;
    c->module          = c->types[id].module;
    u32             t  = c->types[id].type;
    const SemaType* st = cgen_type(c, t);
    SemaTypeKind    k  = st->kind;
    if (k == STK_Array) {
        cgen_define_type(c, cgen_canonical(c, st->first_param_type));
    }
    if (k == STK_Function || k == STK_Tuple || k == STK_Plex ||
        k == STK_Union || k == STK_Enum) {
        for (u32 i = 0; i < st->param_count; ++i) {
            cgen_define_type(c, cgen_canonical(c, cgen_field_type(c, t, i)));
        }
        if (k == STK_Function) {
            cgen_define_type(c, cgen_canonical(c, st->return_type));
        }
    }
    if (k == STK_Function) {
        sb_format(&c->out,
                  "typedef %s (*ncg_t%u)(",
                  cgen_ctype(c, st->return_type),
                  id);
        for (u32 i = 0; i < st->param_count; ++i) {
            sb_format(&c->out,
                      "%s%s",
                      i ? ", " : "",
                      cgen_ctype(c, cgen_field_type(c, t, i)));
        }
        if (st->flags & STF_FunctionVarargs) {
            sb_format(&c->out, "%s...", st->param_count ? ", " : "");
        } else if (!st->param_count) {
            sb_append_cstr(&c->out, "void");
        }
        sb_append_cstr(&c->out, ");\n");
    } else if (k == STK_Array || k == STK_Slice || k == STK_Tuple ||
               k == STK_Plex || k == STK_Union || k == STK_Enum) {
        sb_format(&c->out,
                  "typedef %s ncg_t%u {\n",
                  k == STK_Union ? "union" : "struct",
                  id);
        if (k == STK_Array) {
            sb_format(&c->out,
                      "%s data[%u];\n",
                      cgen_ctype(c, st->first_param_type),
                      st->return_type);
        } else if (k == STK_Slice) {
            sb_append_cstr(&c->out, "void* data; uintptr_t count;\n");
        } else {
            if (k == STK_Enum) {
                sb_append_cstr(&c->out, "int64_t tag; union {\n");
            }
            cgen_record_fields(c, t, 0, 0);
            if (k == STK_Enum) {
                sb_append_cstr(&c->out, "} payload;\n");
            }
        }
        sb_format(&c->out,
                  "} %sncg_t%u;\n",
                  st->flags & STF_PlexPacked ? "__attribute__((packed)) " : "",
                  id);
    }
    c->types[id].state = 2;
    c->module          = old;
}

internal CValue cgen_expr(CGen* c, u32 index);
internal CValue cgen_lvalue(CGen* c, u32 index);
internal void   cgen_block(CGen* c, u32 index);
internal void   cgen_value_block(CGen* c, u32 index, CValue result);
internal void   cgen_stmt(CGen* c, u32 index);
internal cstr   cgen_binding(CGen* c, u32 m, u32 b, u32 depth);
internal cstr   cgen_function(CGen* c, u32 m, u32 f)
{
    return CF("ncg_m%u_fn%u", m, f);
}
internal cstr cgen_decl(CGen* c, u32 m, u32 d, u32 depth)
{
    if (depth > 64 || m >= array_count(c->program->modules)) {
        cgen_error(c, "invalid declaration reference", d);
        return "ncg_invalid";
    }
    const FrontEndState* front = &c->program->modules[m].front_end;
    if (d < array_count(front->hir.decl_binding_indices) &&
        front->hir.decl_binding_indices[d] != U32_MAX) {
        return cgen_binding(
            c, m, front->hir.decl_binding_indices[d], depth + 1);
    }
    if (d < array_count(front->sema.decls)) {
        const SemaDecl* decl = &front->sema.decls[d];
        if (decl->import_module_index != U32_MAX) {
            return cgen_decl(c,
                             decl->import_module_index,
                             decl->import_decl_index,
                             depth + 1);
        }
    }
    cgen_error(c, "unresolved declaration", d);
    return "ncg_invalid";
}
internal cstr cgen_binding(CGen* c, u32 m, u32 b, u32 depth)
{
    const FrontEndState* f = &c->program->modules[m].front_end;
    if (depth > 64 || b >= array_count(f->hir.bindings)) {
        cgen_error(c, "invalid binding", b);
        return "ncg_invalid";
    }
    const HirBinding* bind = &f->hir.bindings[b];
    switch (bind->kind) {
    case HIR_BINDING_Function:
        return cgen_function(c, m, bind->target_index);
    case HIR_BINDING_Value:
        {
            const HirValue* v = &f->hir.values[bind->target_index];
            if (v->decl_index < array_count(f->sema.decls)) {
                const SemaDecl* d = &f->sema.decls[v->decl_index];
                if (d->import_module_index != U32_MAX) {
                    return cgen_decl(c,
                                     d->import_module_index,
                                     d->import_decl_index,
                                     depth + 1);
                }
            }
            return CF("ncg_m%u_g%u", m, bind->target_index);
        }
    case HIR_BINDING_Import:
        {
            const HirImport* im = &f->hir.imports[bind->target_index];
            return cgen_decl(c, im->module_index, im->decl_index, depth + 1);
        }
    default:
        cgen_error(c, "non-value binding used as a value", b);
        return "ncg_invalid";
    }
}
internal CValue cgen_ref(CGen* c, const HirExpr* e)
{
    cstr value = NULL;
    switch (e->ref_kind) {
    case HIR_REF_Local:
        for (u32 i = array_count(c->overrides); i > 0; --i) {
            COverride* o = &c->overrides[i - 1];
            if (o->module == c->module && o->local == e->ref_index) {
                return o->value;
            }
        }
        value = CF("ncg_l%u", e->ref_index);
        break;
    case HIR_REF_Binding:
        value = cgen_binding(c, c->module, e->ref_index, 0);
        break;
    case HIR_REF_Decl:
        value = cgen_decl(c, c->module, e->ref_index, 0);
        break;
    default:
        break;
    }
    if (!value) {
        cgen_error(c, "unresolved value", e->ref_index);
        value = "0";
    }
    return (CValue){value,
                    e->ref_kind == HIR_REF_Local &&
                            e->ref_index < array_count(c->local_types) &&
                            c->local_types[e->ref_index] != U32_MAX
                        ? c->local_types[e->ref_index]
                        : e->type_index,
                    false};
}
internal cstr cgen_truth(CGen* c, CValue v)
{
    switch (cgen_kind(c, v.type)) {
    case STK_String:
    case STK_Slice:
        return CF("(%s.count != 0)", v.text);
    case STK_Enum:
        return CF("(%s.tag %s 0)",
                  v.text,
                  cgen_type(c, v.type)->flags & STF_Result ? "==" : "!=");
    default:
        return CF("(%s != 0)", v.text);
    }
}
internal CValue cgen_coerce(CGen* c, CValue v, u32 t)
{
    SemaTypeKind from = cgen_kind(c, v.type), to = cgen_kind(c, t);
    if (cgen_void(c, v.type)) {
        return cgen_temp(c, t, NULL);
    }
    if (t == v.type || (to != STK_Pointer &&
                        cgen_canonical(c, t) == cgen_canonical(c, v.type))) {
        return (CValue){v.text, t, false};
    }
    if (to == STK_Bool) {
        return cgen_temp(c, t, cgen_truth(c, v));
    }
    if (to == STK_Enum) {
        const SemaType* st = cgen_type(c, t);
        if (from == STK_Enum) {
            CValue r = cgen_temp(c, t, NULL);
            OUT("memcpy(&%s, &%s, sizeof(%s) < sizeof(%s) ? sizeof(%s) : "
                "sizeof(%s));\n",
                r.text,
                v.text,
                r.text,
                v.text,
                r.text,
                v.text);
            return r;
        }
        if (st->flags & (STF_Optional | STF_Result)) {
            u32    which = (st->flags & STF_Optional) ? 1 : 0;
            CValue r     = cgen_temp(c, t, NULL);
            OUT("%s.tag = %lld;\n",
                r.text,
                (long long)cgen_sema(c)
                    ->type_param_values[st->first_param_type + which]);
            CValue payload = cgen_coerce(c, v, cgen_field_type(c, t, which));
            if (!cgen_void(c, payload.type)) {
                OUT("%s.payload.f%u = %s;\n", r.text, which, payload.text);
            }
            return r;
        }
        return cgen_temp(
            c, t, CF("(%s){.tag=(int64_t)(%s)}", cgen_ctype(c, t), v.text));
    }
    if (from == STK_Enum) {
        return cgen_temp(c, t, CF("(%s)(%s.tag)", cgen_ctype(c, t), v.text));
    }
    if (to == STK_Pointer && from == STK_DynamicArray) {
        return cgen_temp(c, t, CF("(%s?%s->data:NULL)", v.text, v.text));
    }
    if (to == STK_DynamicArray && from == STK_Pointer) {
        return cgen_temp(c, t, CF("(%s?(NcgArray*)%s-1:NULL)", v.text, v.text));
    }
    if ((to == STK_String || to == STK_Slice) && from == STK_DynamicArray) {
        return cgen_temp(c,
                         t,
                         CF("(%s){%s?%s->data:NULL,%s?%s->count:0}",
                            cgen_ctype(c, t),
                            v.text,
                            v.text,
                            v.text,
                            v.text));
    }
    if (to == STK_String && from == STK_Array) {
        return cgen_temp(c,
                         t,
                         CF("(NerdString){(uint8_t*)%s.data,%u}",
                            v.text,
                            cgen_type(c, v.type)->return_type));
    }
    if (to == STK_Slice && from == STK_Array) {
        return cgen_temp(c,
                         t,
                         CF("(%s){%s.data,%u}",
                            cgen_ctype(c, t),
                            v.text,
                            cgen_type(c, v.type)->return_type));
    }
    if ((to == STK_Slice || to == STK_String) &&
        (from == STK_Slice || from == STK_String)) {
        return cgen_temp(
            c,
            t,
            CF("(%s){%s.data,%s.count}", cgen_ctype(c, t), v.text, v.text));
    }
    if (to == STK_Pointer && from == STK_Pointer) {
        u32             source = cgen_type(c, v.type)->first_param_type,
                        target = cgen_type(c, t)->first_param_type;
        const SemaType* st     = cgen_type(c, source);
        if (st->kind == STK_Plex && cgen_kind(c, target) == STK_Plex) {
            for (u32 i = 0; i < st->plex_use_count; ++i) {
                const SemaPlexUse* use =
                    &cgen_sema(c)->plex_uses[st->first_plex_use + i];
                if (cgen_canonical(c, use->type_index) ==
                    cgen_canonical(c, target)) {
                    return cgen_temp(c,
                                     t,
                                     CF("&((%s*)%s)->f%u",
                                        cgen_ctype(c, source),
                                        v.text,
                                        use->first_field));
                }
                const SemaType* embedded = cgen_type(c, use->type_index);
                for (u32 j = 0; j < embedded->plex_use_count; ++j) {
                    const SemaPlexUse* nested =
                        &cgen_sema(c)->plex_uses[embedded->first_plex_use + j];
                    if (cgen_canonical(c, nested->type_index) ==
                        cgen_canonical(c, target)) {
                        return cgen_temp(
                            c,
                            t,
                            CF("&((%s*)%s)->f%u",
                               cgen_ctype(c, source),
                               v.text,
                               use->first_field + nested->first_field));
                    }
                }
            }
        }
    }
    if ((to == STK_Pointer || to == STK_Box) &&
        (from == STK_String || from == STK_Slice || from == STK_Array)) {
        return cgen_temp(c, t, CF("(void*)%s.data", v.text));
    }
    return cgen_temp(c, t, CF("(%s)(%s)", cgen_ctype(c, t), v.text));
}
internal void cgen_local(CGen* c, u32 i, u32 t)
{
    if (i == U32_MAX || cgen_void(c, t)) {
        return;
    }
    while (array_count(c->locals) <= i) {
        array_push(c->locals, false);
    }
    if (c->locals[i]) {
        return;
    }
    c->locals[i] = true;
    while (array_count(c->local_types) <= i) {
        array_push(c->local_types, U32_MAX);
    }
    c->local_types[i] = t;
    sb_format(&c->declarations,
              "%s ncg_l%u = %s;\n",
              cgen_ctype(c, t),
              i,
              cgen_zero(c, t));
}
internal CValue cgen_lvalue(CGen* c, u32 index)
{
    const HirExpr* e = &cgen_hir(c)->exprs[index];
    if (e->kind == HIR_EXPR_LocalRef ||
        ((e->kind == HIR_EXPR_Field || e->kind == HIR_EXPR_TupleField) &&
         (e->ref_kind == HIR_REF_Binding || e->ref_kind == HIR_REF_Decl))) {
        return cgen_ref(c, e);
    }
    if (e->kind == HIR_EXPR_Unary && e->unary_op == HIR_UNARY_Deref) {
        CValue p = cgen_expr(c, e->operand_expr_index);
        return (CValue){CF("(*(%s*)%s)", cgen_ctype(c, e->type_index), p.text),
                        e->type_index,
                        false};
    }
    if (e->kind == HIR_EXPR_Field || e->kind == HIR_EXPR_TupleField) {
        const HirExpr* base = &cgen_hir(c)->exprs[e->operand_expr_index];
        u32            bt   = base->type_index;
        CValue         v;
        if (cgen_kind(c, bt) == STK_Pointer || cgen_kind(c, bt) == STK_Box) {
            v      = cgen_expr(c, e->operand_expr_index);
            bt     = cgen_type(c, bt)->first_param_type;
            v.text = CF("(*(%s*)%s)", cgen_ctype(c, bt), v.text);
        } else {
            v = cgen_lvalue(c, e->operand_expr_index);
        }
        u32 f = e->kind == HIR_EXPR_TupleField
                    ? (u32)e->integer
                    : cgen_field_index(c, bt, e->symbol_handle);
        if (f != U32_MAX) {
            return (CValue){CF("(%s).f%u", v.text, f),
                            e->type_index,
                            cgen_bitfield(c, bt, f)};
        }
        cstr name = cgen_symbol(c, e->symbol_handle);
        return (CValue){CF("(%s).%s", v.text, name), e->type_index, false};
    }
    if (e->kind == HIR_EXPR_Index) {
        u32    bt   = cgen_hir(c)->exprs[e->operand_expr_index].type_index;
        CValue b    = cgen_kind(c, bt) == STK_Array
                          ? cgen_lvalue(c, e->operand_expr_index)
                          : cgen_expr(c, e->operand_expr_index);
        CValue i    = cgen_expr(c, e->extra_expr_index);
        cstr   data = b.text;
        if (cgen_kind(c, bt) == STK_Array || cgen_kind(c, bt) == STK_Slice ||
            cgen_kind(c, bt) == STK_String) {
            data = CF("(%s).data", b.text);
        } else if (cgen_kind(c, bt) == STK_DynamicArray) {
            data = CF("(%s)->data", b.text);
        }
        return (CValue){
            CF("((%s*)%s)[%s]", cgen_ctype(c, e->type_index), data, i.text),
            e->type_index,
            false};
    }
    return cgen_expr(c, index);
}

internal CValue cgen_expr_as(CGen* c, u32 index, u32 type)
{
    u32 saved        = c->expected_type;
    c->expected_type = type;
    CValue value     = cgen_expr(c, index);
    c->expected_type = saved;
    return cgen_coerce(c, value, type);
}
internal void cgen_cleanup_to(CGen* c, u32 base)
{
    for (u32 n = array_count(c->cleanups); n > base; --n) {
        CCleanup cl                = c->cleanups[n - 1];
        u32      save              = array_count(c->cleanups);
        __array_count(c->cleanups) = n - 1;
        if (cl.block) {
            cgen_block(c, cl.index);
        } else {
            OUT("nrt_mem_free(ncg_l%u); ncg_l%u = NULL;\n", cl.index, cl.index);
        }
        __array_count(c->cleanups) = save;
    }
}
internal void cgen_consume(CGen* c, u32 index, u32 t)
{
    if (cgen_kind(c, t) != STK_Box || index == U32_MAX) {
        return;
    }
    const HirExpr* e = &cgen_hir(c)->exprs[index];
    if (e->kind == HIR_EXPR_LocalRef && e->ref_kind == HIR_REF_Local) {
        OUT("ncg_l%u = NULL;\n", e->ref_index);
    }
}
internal void cgen_return(CGen* c, CValue v)
{
    if (!cgen_void(c, c->return_type)) {
        v = cgen_coerce(c, v, c->return_type);
    }
    cgen_cleanup_to(c, 0);
    if (c->va_local != U32_MAX) {
        OUT("nrt_va_done(ncg_l%u);\n", c->va_local);
    }
    if (cgen_void(c, c->return_type)) {
        OUT("return;\n");
    } else {
        OUT("return %s;\n", v.text);
    }
}
internal CValue cgen_enum(CGen* c, const HirExpr* e, u32 variant)
{
    CValue          result = cgen_temp(c, e->type_index, NULL);
    const SemaType* st     = cgen_type(c, e->type_index);
    OUT("%s.tag=%lld;\n",
        result.text,
        (long long)cgen_sema(c)
            ->type_param_values[st->first_param_type + variant]);
    u32 pt = cgen_field_type(c, e->type_index, variant);
    if (e->arg_count && !cgen_void(c, pt)) {
        if ((cgen_kind(c, pt) == STK_Tuple || cgen_kind(c, pt) == STK_Plex) &&
            !(e->arg_count == 1 &&
              cgen_canonical(
                  c,
                  cgen_hir(c)
                      ->exprs[cgen_hir(c)->call_args[e->first_arg].expr_index]
                      .type_index) == cgen_canonical(c, pt))) {
            for (u32 i = 0; i < e->arg_count; ++i) {
                const HirCallArg* a = &cgen_hir(c)->call_args[e->first_arg + i];
                u32    fi = a->symbol_handle == U32_MAX
                                ? i
                                : cgen_field_index(c, pt, a->symbol_handle);
                CValue v  = cgen_coerce(
                    c, cgen_expr(c, a->expr_index), cgen_field_type(c, pt, fi));
                OUT("%s.payload.f%u.f%u=%s;\n",
                    result.text,
                    variant,
                    fi,
                    v.text);
            }
        } else {
            CValue v = cgen_coerce(
                c,
                cgen_expr(c, cgen_hir(c)->call_args[e->first_arg].expr_index),
                pt);
            OUT("%s.payload.f%u=%s;\n", result.text, variant, v.text);
        }
    }
    return result;
}
internal cstr cgen_equal(CGen* c, CValue a, CValue b, u32 depth)
{
    if (depth > 64) {
        cgen_error(c, "recursive equality", a.type);
        return "false";
    }
    SemaTypeKind k = cgen_kind(c, a.type);
    if (k == STK_String) {
        return CF("nrt_string_eq(&%s,&%s)", a.text, b.text);
    }
    if (k == STK_Enum) {
        return CF("(%s.tag==%s.tag)", a.text, b.text);
    }
    if (k == STK_Array || k == STK_Slice || k == STK_DynamicArray) {
        u32  item = cgen_type(c, a.type)->first_param_type;
        cstr ac   = k == STK_Array ? CF("%u", cgen_type(c, a.type)->return_type)
                    : k == STK_DynamicArray
                        ? CF("(%s?%s->count:0)", a.text, a.text)
                        : CF("%s.count", a.text);
        cstr bc   = k == STK_Array ? ac
                    : k == STK_DynamicArray
                        ? CF("(%s?%s->count:0)", b.text, b.text)
                        : CF("%s.count", b.text);
        cstr ad   = k == STK_DynamicArray
                        ? CF("(%s?%s->data:NULL)", a.text, a.text)
                        : CF("%s.data", a.text);
        cstr bd   = k == STK_DynamicArray
                        ? CF("(%s?%s->data:NULL)", b.text, b.text)
                        : CF("%s.data", b.text);
        cstr r = CF("ncg_eq%u", c->next++), i = CF("ncg_i%u", c->next++);
        OUT("bool %s = %s == %s;\nfor(uintptr_t %s=0; %s && %s<%s; ++%s) {\n",
            r,
            ac,
            bc,
            i,
            r,
            i,
            ac,
            i);
        CValue x =
            cgen_temp(c, item, CF("((%s*)%s)[%s]", cgen_ctype(c, item), ad, i));
        CValue y =
            cgen_temp(c, item, CF("((%s*)%s)[%s]", cgen_ctype(c, item), bd, i));
        OUT("%s=%s;\n}\n", r, cgen_equal(c, x, y, depth + 1));
        return r;
    }
    if (k == STK_Box) {
        u32          item = cgen_type(c, a.type)->first_param_type;
        SemaTypeKind ik   = cgen_kind(c, item);
        bool supported = ik != STK_Plex && ik != STK_Union && ik != STK_Arena;
        for (u32 i = 0; i < array_count(cgen_hir(c)->equality_methods); ++i) {
            if (cgen_hir(c)->equality_methods[i].type_index == item) {
                supported = true;
            }
        }
        if (!supported) {
            return CF("(%s==%s)", a.text, b.text);
        }
        cstr r = CF("ncg_eq%u", c->next++), n = CF("ncg_n%u", c->next++),
             i = CF("ncg_i%u", c->next++);
        OUT("bool %s=%s==%s; if(%s && %s) { uintptr_t "
            "%s=nrt_mem_size(%s)/sizeof(%s); "
            "%s=%s==nrt_mem_size(%s)/sizeof(%s);\nfor(uintptr_t %s=0;%s && "
            "%s<%s;++%s) {\n",
            r,
            a.text,
            b.text,
            a.text,
            b.text,
            n,
            a.text,
            cgen_ctype(c, item),
            r,
            n,
            b.text,
            cgen_ctype(c, item),
            i,
            r,
            i,
            n,
            i);
        CValue x = cgen_temp(
            c, item, CF("((%s*)%s)[%s]", cgen_ctype(c, item), a.text, i));
        CValue y = cgen_temp(
            c, item, CF("((%s*)%s)[%s]", cgen_ctype(c, item), b.text, i));
        OUT("%s=%s; }}\n", r, cgen_equal(c, x, y, depth + 1));
        return r;
    }
    for (u32 j = 0; j < array_count(cgen_hir(c)->equality_methods); ++j) {
        const HirEqualityMethod* method = &cgen_hir(c)->equality_methods[j];
        if (method->type_index == a.type) {
            CValue fn = cgen_expr(c, method->callee_expr_index);
            return CF("%s(%s,%s)", fn.text, a.text, b.text);
        }
    }
    return CF("(%s==%s)", a.text, b.text);
}
internal cstr cgen_pattern(CGen* c, u32 index, CValue value)
{
    const HirPattern* p = &cgen_hir(c)->patterns[index];
    if (p->kind == HIR_PATTERN_Ignore) {
        return "true";
    }
    if (p->kind == HIR_PATTERN_Bind) {
        cgen_local(c, p->local_index, value.type);
        OUT("ncg_l%u=%s;\n", p->local_index, value.text);
        return "true";
    }
    bool braced = false;
    if (p->kind == HIR_PATTERN_Tuple || p->kind == HIR_PATTERN_Plex ||
        p->kind == HIR_PATTERN_EnumVariant) {
        cstr condition = "true";
        if (p->kind == HIR_PATTERN_EnumVariant) {
            u32 v = cgen_field_index(c, value.type, p->symbol_handle);
            if (v == U32_MAX) {
                cgen_error(c, "enum pattern", index);
                return "false";
            }
            braced = cgen_sema(c)->type_param_braced_payloads
                         [cgen_type(c, value.type)->first_param_type + v];
            condition =
                CF("(%s.tag==%lld)",
                   value.text,
                   (long long)cgen_sema(c)->type_param_values
                       [cgen_type(c, value.type)->first_param_type + v]);
            value = (CValue){CF("%s.payload.f%u", value.text, v),
                             cgen_field_type(c, value.type, v),
                             false};
        }
        cstr result = CF("ncg_match%u", c->next++);
        OUT("bool %s=%s; if(%s) {\n", result, condition, result);
        for (u32 i = 0; i < p->child_count; ++i) {
            const HirPatternChild* ch =
                &cgen_hir(c)->pattern_children[p->first_child + i];
            CValue child = value;
            if ((cgen_kind(c, value.type) == STK_Plex ||
                 cgen_kind(c, value.type) == STK_Tuple) &&
                !(p->kind == HIR_PATTERN_EnumVariant && !braced &&
                  p->child_count == 1 &&
                  cgen_hir(c)->patterns[ch->pattern_index].kind ==
                      HIR_PATTERN_Bind)) {
                u32 fi =
                    ch->has_field_name
                        ? cgen_field_index(c, value.type, ch->symbol_handle)
                        : i;
                child = (CValue){CF("%s.f%u", value.text, fi),
                                 cgen_field_type(c, value.type, fi),
                                 false};
            }
            cstr test = cgen_pattern(c, ch->pattern_index, child);
            OUT("%s = %s && %s;\n", result, result, test);
        }
        OUT("}\n");
        return result;
    }
    CValue rhs = cgen_coerce(c, cgen_expr(c, p->expr_index), value.type);
    if (p->kind == HIR_PATTERN_RangeExclusive ||
        p->kind == HIR_PATTERN_RangeInclusive) {
        CValue end =
            cgen_coerce(c, cgen_expr(c, p->extra_expr_index), value.type);
        return CF("(%s>=%s && %s%s%s)",
                  value.text,
                  rhs.text,
                  value.text,
                  p->kind == HIR_PATTERN_RangeInclusive ? "<=" : "<",
                  end.text);
    }
    if (p->kind == HIR_PATTERN_Value || p->kind == HIR_PATTERN_Equal) {
        return cgen_equal(c, value, rhs, 0);
    }
    if (p->kind == HIR_PATTERN_NotEqual) {
        return CF("!(%s)", cgen_equal(c, value, rhs, 0));
    }
    cstr op = p->kind == HIR_PATTERN_Less        ? "<"
              : p->kind == HIR_PATTERN_LessEqual ? "<="
              : p->kind == HIR_PATTERN_Greater   ? ">"
                                                 : ">=";
    return CF("(%s%s%s)", value.text, op, rhs.text);
}
internal CValue cgen_control(CGen* c, const HirExpr* e)
{
    CValue  result = cgen_temp(c, e->type_index, NULL);
    u32     id     = c->next++;
    CTarget target = {e->symbol_handle,
                      id,
                      e->type_index,
                      (u32)array_count(c->cleanups),
                      cgen_void(c, e->type_index) ? NULL : result.text,
                      false};
    if (e->kind == HIR_EXPR_Block) {
        array_push(c->targets, target);
        cgen_value_block(c, e->body_block_index, result);
        array_pop(c->targets);
        OUT("ncg_end%u:;\n", id);
        return result;
    }
    if (e->kind == HIR_EXPR_On) {
        CValue subject = {0};
        if (e->operand_expr_index != U32_MAX) {
            subject = cgen_expr(c, e->operand_expr_index);
        }
        if (e->on_kind == HIR_ON_Bool) {
            subject = cgen_coerce(c, subject, cgen_builtin(c, STK_Bool));
        }
        bool push = !cgen_void(c, e->type_index);
        if (push) {
            array_push(c->targets, target);
        }
        for (u32 i = 0; i < e->branch_count; ++i) {
            const HirOnBranch* b =
                &cgen_hir(c)->on_branches[e->first_branch + i];
            cstr cond = "true";
            if (!b->is_else) {
                cstr matched = CF("ncg_match%u", c->next++);
                OUT("bool %s=%s;\n",
                    matched,
                    b->pattern_count ? "false" : "true");
                for (u32 j = 0; j < b->pattern_count; ++j) {
                    OUT("if(!%s) {\n", matched);
                    u32 pi =
                        cgen_hir(c)->on_branch_patterns[b->first_pattern + j];
                    if (e->on_kind == HIR_ON_Condition) {
                        CValue cv =
                            cgen_expr(c, cgen_hir(c)->patterns[pi].expr_index);
                        cond = cgen_truth(c, cv);
                    } else {
                        cond = cgen_pattern(c, pi, subject);
                    }
                    OUT("%s=%s; }\n", matched, cond);
                }
                cond = matched;
            }
            OUT("if(%s) {\n", cond);
            if (b->binder_local_index != U32_MAX && subject.text) {
                cgen_local(c, b->binder_local_index, subject.type);
                OUT("ncg_l%u=%s;\n", b->binder_local_index, subject.text);
            }
            if (b->guard_expr_index != U32_MAX) {
                CValue guard = cgen_expr(c, b->guard_expr_index);
                OUT("if(%s) {\n", cgen_truth(c, guard));
            }
            cgen_value_block(c, b->body_block_index, result);
            OUT("goto ncg_end%u;\n", id);
            if (b->guard_expr_index != U32_MAX) {
                OUT("}\n");
            }
            OUT("}\n");
        }
        if (push) {
            array_pop(c->targets);
        }
        OUT("ncg_end%u:;\n", id);
        return result;
    }
    const HirFor* loop = &cgen_hir(c)->fors[e->for_index];
    target.symbol      = loop->label_symbol;
    target.loop        = true;
    for (u32 i = 0; i < loop->init_stmt_count; ++i) {
        cgen_stmt(c, cgen_hir(c)->for_init_stmts[loop->first_init_stmt + i]);
    }
    CValue iterable = {0};
    cstr   counter = CF("ncg_iter%u", id), end = NULL;
    bool   range       = false;
    cstr   range_start = "0";
    bool   custom      = loop->iterator_next_decl_index != U32_MAX;
    if (loop->kind == HIR_FOR_In && !custom) {
        const HirExpr* ie = &cgen_hir(c)->exprs[loop->iterable_expr_index];
        range             = ie->kind == HIR_EXPR_RangeExclusive ||
                            ie->kind == HIR_EXPR_RangeInclusive;
        if (range) {
            CValue start  = cgen_expr(c, ie->lhs_expr_index),
                   finish = cgen_expr(c, ie->rhs_expr_index);
            range_start   = start.text;
            OUT("%s %s=%s;\n", cgen_ctype(c, start.type), counter, start.text);
            end = CF("%s%s%s",
                     counter,
                     ie->kind == HIR_EXPR_RangeInclusive ? "<=" : "<",
                     finish.text);
        } else {
            iterable = cgen_kind(c, ie->type_index) == STK_Array
                           ? cgen_lvalue(c, loop->iterable_expr_index)
                           : cgen_expr(c, loop->iterable_expr_index);
            cstr count =
                cgen_kind(c, iterable.type) == STK_Array
                    ? CF("%u", cgen_type(c, iterable.type)->return_type)
                : cgen_kind(c, iterable.type) == STK_DynamicArray
                    ? CF("(%s?%s->count:0)", iterable.text, iterable.text)
                    : CF("%s.count", iterable.text);
            OUT("uintptr_t %s=0;\n", counter);
            end = CF("%s<%s", counter, count);
        }
    }
    CValue iterator  = {0};
    u32    next_type = U32_MAX;
    if (custom) {
        iterator = cgen_lvalue(c, loop->iterable_expr_index);
        next_type =
            cgen_sema(c)->decls[loop->iterator_next_decl_index].type_index;
    }
    OUT("ncg_cond%u:;\n", id);
    if (loop->condition_expr_index != U32_MAX) {
        CValue condition = cgen_expr(c, loop->condition_expr_index);
        end              = cgen_truth(c, condition);
    }
    if (end) {
        OUT("if(!(%s)) goto ncg_else%u;\n", end, id);
    }
    if (custom) {
        CValue next = cgen_temp(
            c,
            cgen_type(c, next_type)->return_type,
            CF("%s(&(%s))",
               cgen_decl(c, c->module, loop->iterator_next_decl_index, 0),
               iterator.text));
        OUT("if(%s.tag!=1)goto ncg_else%u;\n", next.text, id);
        u32 it = cgen_field_type(c, next.type, 1);
        cgen_local(c, loop->item_local_index, it);
        OUT("ncg_l%u=%s.payload.f1;\n", loop->item_local_index, next.text);
    }
    if (loop->kind == HIR_FOR_In && !custom) {
        if (loop->index_local_index != U32_MAX) {
            u32 t = cgen_sema(c)->locals[loop->index_local_index].type_index;
            cgen_local(c, loop->index_local_index, t);
            OUT("ncg_l%u=%s;\n",
                loop->index_local_index,
                range ? CF("%s-%s", counter, range_start) : counter);
        }
        if (loop->item_local_index != U32_MAX) {
            u32 t = cgen_sema(c)->locals[loop->item_local_index].type_index;
            cgen_local(c, loop->item_local_index, t);
            if (range) {
                OUT("ncg_l%u=%s;\n", loop->item_local_index, counter);
            } else {
                cstr data = cgen_kind(c, iterable.type) == STK_DynamicArray
                                ? CF("%s->data", iterable.text)
                                : CF("%s.data", iterable.text);
                OUT("ncg_l%u=&((%s*)%s)[%s];\n",
                    loop->item_local_index,
                    cgen_ctype(c, cgen_type(c, t)->first_param_type),
                    data,
                    counter);
            }
        }
    }
    array_push(c->targets, target);
    cgen_block(c, loop->body_block_index);
    OUT("ncg_continue%u:;\n", id);
    for (u32 i = 0; i < loop->update_stmt_count; ++i) {
        cgen_stmt(c,
                  cgen_hir(c)->for_update_stmts[loop->first_update_stmt + i]);
    }
    if (loop->kind == HIR_FOR_In && !custom) {
        OUT("++%s;\n", counter);
    }
    OUT("goto ncg_cond%u;\nncg_else%u:;\n", id, id);
    if (loop->else_block_index != U32_MAX) {
        cgen_value_block(c, loop->else_block_index, result);
    }
    array_pop(c->targets);
    OUT("ncg_end%u:;\n", id);
    return result;
}

// C layout equivalence is weaker than semantic identity (e.g. []i32 and
// []string). Keep generic selection on the original semantic types.
internal bool cgen_same_type(CGen* c, u32 am, u32 a, u32 bm, u32 b, u32 depth)
{
    if (am == bm && a == b) {
        return true;
    }
    const FrontEndState* af = &c->program->modules[am].front_end;
    const FrontEndState* bf = &c->program->modules[bm].front_end;
    if (a >= array_count(af->sema.types) || b >= array_count(bf->sema.types)) {
        return a == b;
    }
    const SemaType *at = &af->sema.types[a], *bt = &bf->sema.types[b];
    if (at->kind != bt->kind || at->flags != bt->flags ||
        at->param_count != bt->param_count) {
        return false;
    }
    if (depth > 64) {
        return true;
    }
    switch (at->kind) {
    case STK_Array:
        if (at->return_type != bt->return_type) {
            return false;
        }
        [[fallthrough]];
    case STK_Pointer:
    case STK_Box:
    case STK_Slice:
    case STK_DynamicArray:
    case STK_Atomic:
        return cgen_same_type(
            c, am, at->first_param_type, bm, bt->first_param_type, depth + 1);
    case STK_Function:
        if (!cgen_same_type(
                c, am, at->return_type, bm, bt->return_type, depth + 1)) {
            return false;
        }
        [[fallthrough]];
    case STK_Tuple:
    case STK_Plex:
    case STK_Union:
    case STK_Enum:
        for (u32 i = 0; i < at->param_count; ++i) {
            u32 ap = at->first_param_type + i, bp = bt->first_param_type + i;
            if (!cgen_same_type(c,
                                am,
                                af->sema.type_param_types[ap],
                                bm,
                                bf->sema.type_param_types[bp],
                                depth + 1)) {
                return false;
            }
            if (at->kind == STK_Plex || at->kind == STK_Union ||
                at->kind == STK_Enum) {
                if (!string_eq(
                        lex_symbol(&af->lexer, af->sema.type_param_symbols[ap]),
                        lex_symbol(&bf->lexer,
                                   bf->sema.type_param_symbols[bp]))) {
                    return false;
                }
            }
        }
        return true;
    default:
        return true;
    }
}
internal u32 cgen_expected(CGen* c, u32 t)
{
    return t == U32_MAX ? U32_MAX : c->offsets[c->module] + t;
}
internal bool cgen_matches(CGen* c, u32 type, u32 expected)
{
    if (expected == U32_MAX) {
        return true;
    }
    for (u32 m = 0; m < array_count(c->program->modules); ++m) {
        u32 count = array_count(c->program->modules[m].front_end.sema.types);
        if (expected >= c->offsets[m] && expected - c->offsets[m] < count) {
            return cgen_same_type(
                c, c->module, type, m, expected - c->offsets[m], 0);
        }
    }
    return false;
}

// Resolve a callable through HIR bindings, imports, local functions and checked
// instantiations. Parameter values remain ordinary indirect calls.
internal bool cgen_resolve(CGen* c,
                           u32   module,
                           u32   index,
                           u32   expected,
                           u32*  out_module,
                           u32*  out_fn,
                           u32   depth)
{
    if (depth > 64) {
        return false;
    }
    u32 saved        = c->module;
    c->module        = module;
    const Hir*  h    = cgen_hir(c);
    const Sema* sema = cgen_sema(c);
    if (index >= array_count(h->exprs)) {
        c->module = saved;
        return false;
    }
    const HirExpr* e    = &h->exprs[index];
    u32            decl = U32_MAX, binding = U32_MAX;
    if (e->kind == HIR_EXPR_FunctionRef) {
        *out_module = module;
        *out_fn     = e->ref_index;
        c->module   = saved;
        return true;
    }
    if (e->kind == HIR_EXPR_Index &&
        cgen_kind(c, e->type_index) == STK_Function) {
        bool ok   = cgen_resolve(c,
                                 module,
                                 e->operand_expr_index,
                                 cgen_expected(c, e->type_index),
                                 out_module,
                                 out_fn,
                                 depth + 1);
        c->module = saved;
        return ok;
    }
    if (e->kind == HIR_EXPR_LocalRef) {
        if (e->ref_kind == HIR_REF_Binding) {
            binding = e->ref_index;
        } else if (e->ref_kind == HIR_REF_Decl) {
            decl = e->ref_index;
        } else if (e->ref_kind == HIR_REF_Local &&
                   e->ref_index < array_count(sema->locals) &&
                   cgen_kind(c, sema->locals[e->ref_index].type_index) ==
                       STK_Function) {
            for (u32 i = 0; i < array_count(h->stmts); ++i) {
                const HirStmt* st = &h->stmts[i];
                if (st->kind == HIR_STMT_Let &&
                    st->local_index == e->ref_index) {
                    bool ok   = cgen_resolve(c,
                                             module,
                                             st->expr_index,
                                             expected,
                                             out_module,
                                             out_fn,
                                             depth + 1);
                    c->module = saved;
                    return ok;
                }
            }
        }
    } else if (e->kind == HIR_EXPR_Field) {
        u32 owner = h->exprs[e->operand_expr_index].type_index;
        if (cgen_kind(c, owner) == STK_Module) {
            u32                  other = cgen_type(c, owner)->return_type;
            const FrontEndState* f     = &c->program->modules[other].front_end;
            string name = lex_symbol(&cgen_front(c)->lexer, e->symbol_handle);
            for (u32 i = 0; i < array_count(f->hir.bindings); ++i) {
                if (string_eq(name,
                              lex_symbol(&f->lexer,
                                         f->hir.bindings[i].symbol_handle))) {
                    module    = other;
                    h         = &f->hir;
                    sema      = &f->sema;
                    c->module = other;
                    binding   = i;
                    break;
                }
            }
        } else {
            for (u32 i = 0; i < array_count(sema->methods); ++i) {
                const SemaMethod* method = &sema->methods[i];
                if (method->symbol_handle == e->symbol_handle &&
                    method->decl_index < array_count(sema->decls)) {
                    for (u32 fi = 0; fi < array_count(h->functions); ++fi) {
                        const HirFunction* f = &h->functions[fi];
                        if ((f->decl_index == method->decl_index ||
                             f->fn_node_index == sema->decls[method->decl_index]
                                                     .value_node_index) &&
                            cgen_matches(c, f->type_index, expected)) {
                            *out_module = module;
                            *out_fn     = fi;
                            c->module   = saved;
                            return true;
                        }
                    }
                    if (cgen_matches(c,
                                     sema->decls[method->decl_index].type_index,
                                     expected)) {
                        decl = method->decl_index;
                        break;
                    }
                }
            }
        }
    }
    for (u32 n = 0; n < 64; ++n) {
        if (decl < array_count(sema->decls)) {
            const SemaDecl* d = &sema->decls[decl];
            if (d->import_module_index != U32_MAX) {
                module    = d->import_module_index;
                decl      = d->import_decl_index;
                c->module = module;
                h         = cgen_hir(c);
                sema      = cgen_sema(c);
                continue;
            }
            for (u32 f = 0; f < array_count(h->functions); ++f) {
                const HirFunction* fn = &h->functions[f];
                if ((fn->decl_index == decl ||
                     fn->fn_node_index == d->value_node_index) &&
                    (expected == U32_MAX ||
                     cgen_matches(c, fn->type_index, expected))) {
                    *out_module = module;
                    *out_fn     = f;
                    c->module   = saved;
                    return true;
                }
            }
            binding = decl < array_count(h->decl_binding_indices)
                          ? h->decl_binding_indices[decl]
                          : U32_MAX;
            decl    = U32_MAX;
        }
        if (binding >= array_count(h->bindings)) {
            break;
        }
        const HirBinding* b = &h->bindings[binding];
        binding             = U32_MAX;
        if (b->kind == HIR_BINDING_Function) {
            *out_module = module;
            *out_fn     = b->target_index;
            c->module   = saved;
            return true;
        }
        if (b->kind == HIR_BINDING_Import) {
            const HirImport* im = &h->imports[b->target_index];
            module              = im->module_index;
            decl                = im->decl_index;
            c->module           = module;
            h                   = cgen_hir(c);
            sema                = cgen_sema(c);
            continue;
        }
        if (b->kind == HIR_BINDING_Value) {
            bool ok = cgen_resolve(c,
                                   module,
                                   h->values[b->target_index].value_expr_index,
                                   expected,
                                   out_module,
                                   out_fn,
                                   depth + 1);
            c->module = saved;
            return ok;
        }
        break;
    }
    c->module = saved;
    return false;
}
internal u32 cgen_translate_type(CGen* c, u32 module, u32 type)
{
    u32 saved = c->module;
    c->module = module;
    u32 id    = cgen_canonical(c, type);
    c->module = saved;
    if (id == U32_MAX) {
        return U32_MAX;
    }
    for (u32 i = 0; i < array_count(cgen_sema(c)->types); ++i) {
        if (cgen_canonical(c, i) == id) {
            return i;
        }
    }
    return U32_MAX;
}

internal void cgen_display(CGen* c, CValue value, u32 depth)
{
    if (depth > 64) {
        cgen_error(c, "recursive display", value.type);
        return;
    }
    SemaTypeKind k = cgen_kind(c, value.type);
    for (u32 i = 0; i < array_count(cgen_sema(c)->methods); ++i) {
        const SemaMethod* method = &cgen_sema(c)->methods[i];
        if (!method->is_trait_impl ||
            strcmp(cgen_symbol(c, method->symbol_handle), "show") != 0 ||
            method->decl_index >= array_count(cgen_sema(c)->decls)) {
            continue;
        }
        const SemaDecl* d  = &cgen_sema(c)->decls[method->decl_index];
        const SemaType* fn = cgen_type(c, d->type_index);
        if (fn->kind != STK_Function || fn->param_count != 1 ||
            cgen_kind(c, fn->return_type) != STK_String ||
            cgen_canonical(c, cgen_field_type(c, d->type_index, 0)) !=
                cgen_canonical(c, value.type)) {
            continue;
        }
        cstr name = cgen_symbol(c, d->symbol_handle);
        if (strstr(name, "Display") == NULL) {
            continue;
        }
        CValue text =
            cgen_temp(c,
                      fn->return_type,
                      CF("%s(%s)",
                         cgen_decl(c, c->module, method->decl_index, 0),
                         value.text));
        OUT("nrt_string_builder_append_string(&%s);\n", text.text);
        return;
    }
    if (k == STK_Tuple || k == STK_Plex) {
        OUT("nrt_string_builder_append_byte('(');\n");
        for (u32 i = 0; i < cgen_type(c, value.type)->param_count; ++i) {
            if (i) {
                OUT("nrt_string_builder_append_byte(','); "
                    "nrt_string_builder_append_byte(' ');\n");
            }
            CValue v = cgen_temp(c,
                                 cgen_field_type(c, value.type, i),
                                 CF("%s.f%u", value.text, i));
            cgen_display(c, v, depth + 1);
        }
        if (cgen_type(c, value.type)->param_count == 1) {
            OUT("nrt_string_builder_append_byte(',');\n");
        }
        OUT("nrt_string_builder_append_byte(')');\n");
        return;
    }
    if (k == STK_Array || k == STK_Slice || k == STK_DynamicArray) {
        u32  it    = cgen_type(c, value.type)->first_param_type;
        cstr count = k == STK_Array
                         ? CF("%u", cgen_type(c, value.type)->return_type)
                     : k == STK_DynamicArray
                         ? CF("(%s?%s->count:0)", value.text, value.text)
                         : CF("%s.count", value.text);
        cstr data  = k == STK_DynamicArray ? CF("%s->data", value.text)
                                           : CF("%s.data", value.text),
             i     = CF("ncg_show%u", c->next++);
        OUT("nrt_string_builder_append_byte('['); for(uintptr_t "
            "%s=0;%s<%s;++%s) "
            "{\nif(%s){nrt_string_builder_append_byte(',');nrt_string_builder_"
            "append_byte(' ');}\n",
            i,
            i,
            count,
            i,
            i);
        CValue v =
            cgen_temp(c, it, CF("((%s*)%s)[%s]", cgen_ctype(c, it), data, i));
        cgen_display(c, v, depth + 1);
        OUT("} nrt_string_builder_append_byte(']');\n");
        return;
    }
    if (k == STK_Enum) {
        value =
            (CValue){CF("%s.tag", value.text), cgen_builtin(c, STK_I64), false};
        k = STK_I64;
    }
    if (k != STK_String) {
        cstr name = k == STK_Bool                           ? "bool"
                    : k == STK_F32                          ? "f32"
                    : k == STK_F64 || k == STK_UntypedFloat ? "f64"
                    : k == STK_U64 || k == STK_Usize || k == STK_Pointer
                        ? "u64"
                        : "i64";
        cstr str  = CF("ncg_str%u", c->next++);
        OUT("NerdString %s={0}; nrt_to_string_%s(&%s,%s);\n",
            str,
            name,
            str,
            k == STK_Pointer ? CF("(uintptr_t)%s", value.text) : value.text);
        value.text = str;
    }
    OUT("nrt_string_builder_append_string(&%s);\n", value.text);
}

internal CValue cgen_array_call(CGen*          c,
                                const HirExpr* e,
                                const HirExpr* callee)
{
    CValue receiver = cgen_lvalue(c, callee->operand_expr_index);
    u32    item     = cgen_type(c, receiver.type)->first_param_type;
    cstr   ptr      = CF("ncg_array%u", c->next++);
    OUT("NcgArray** %s=&(%s);\n", ptr, receiver.text);
    cstr a = CF("(*%s)", ptr), method = cgen_symbol(c, callee->symbol_handle),
         it            = cgen_ctype(c, item);
    Array(CValue) args = NULL;
    for (u32 i = 0; i < e->arg_count; ++i) {
        array_push(
            args,
            cgen_expr(c, cgen_hir(c)->call_args[e->first_arg + i].expr_index));
    }
    CValue result = {"0", e->type_index, false};
    cstr   file   = cgen_quote(c, e->source_path);
    if (strcmp(method, "push") == 0 || strcmp(method, "append") == 0 ||
        strncmp(method, "reserve_", 8) == 0 ||
        strncmp(method, "resize", 6) == 0 ||
        strncmp(method, "extend", 6) == 0) {
        OUT("if(!%s)ncg_reserve(%s,0,sizeof(%s),_Alignof(%s),%s,%u);\n",
            a,
            ptr,
            it,
            it,
            file,
            e->source_line);
    }
    if (strcmp(method, "free") == 0) {
        OUT("ncg_array_free(%s);\n", ptr);
    } else if (strcmp(method, "clear") == 0) {
        OUT("if(%s)%s->count=0;\n", a, a);
    } else if (strcmp(method, "push") == 0) {
        CValue v = cgen_coerce(c, args[0], item);
        OUT("ncg_reserve(%s,(%s->count<%s->capacity?%s->capacity:(%s->capacity?"
            "%s->capacity*2:1)),sizeof(%s),_Alignof(%s),%s,%u);"
            "\n",
            ptr,
            a,
            a,
            a,
            a,
            a,
            it,
            it,
            file,
            e->source_line);
        OUT("((%s*)%s->data)[%s->count++]=%s;\n", it, a, a, v.text);
    } else if (strcmp(method, "pop") == 0) {
        result = cgen_temp(c, item, NULL);
        OUT("if(%s && %s->count) %s=((%s*)%s->data)[--%s->count];\n",
            a,
            a,
            result.text,
            it,
            a,
            a);
    } else if (strcmp(method, "delete") == 0 ||
               strcmp(method, "swap_delete") == 0) {
        OUT("if(%s && %s<%s->count) {\n", a, args[0].text, a);
        if (strcmp(method, "delete") == 0) {
            OUT("memmove((%s*)%s->data+%s,(%s*)%s->data+%s+1,(%s->count-%s-1)*"
                "sizeof(%s));\n",
                it,
                a,
                args[0].text,
                it,
                a,
                args[0].text,
                a,
                args[0].text,
                it);
        } else {
            OUT("((%s*)%s->data)[%s]=((%s*)%s->data)[%s->count-1];\n",
                it,
                a,
                args[0].text,
                it,
                a,
                a);
        }
        OUT("--%s->count; }\n", a);
    } else if (strcmp(method, "reserve_to") == 0 ||
               strcmp(method, "reserve_extra") == 0) {
        cstr cap = strcmp(method, "reserve_extra") == 0
                       ? CF("(%s?%s->count:0)+%s", a, a, args[0].text)
                       : args[0].text;
        OUT("ncg_reserve(%s,%s,sizeof(%s),_Alignof(%s),%s,%u);\n",
            ptr,
            cap,
            it,
            it,
            file,
            e->source_line);
    } else if (strcmp(method, "append") == 0) {
        cstr count = CF("ncg_count%u", c->next++);
        OUT("uintptr_t %s=%s.count;\n", count, args[0].text);
        OUT("ncg_reserve(%s,(%s?%s->count:0)+%s,sizeof(%s),_Alignof(%s),%s,%u);"
            "\n",
            ptr,
            a,
            a,
            count,
            it,
            it,
            file,
            e->source_line);
        OUT("memmove((%s*)%s->data+%s->count,%s.data,%s*sizeof(%s));%s->count+="
            "%s;\n",
            it,
            a,
            a,
            args[0].text,
            count,
            it,
            a,
            count);
    } else {
        bool extend    = strncmp(method, "extend", 6) == 0;
        bool undefined = strstr(method, "undefined") != NULL;
        if (!extend && strncmp(method, "resize", 6) != 0) {
            cgen_error(c, "unknown array method", e->callee_expr_index);
        }
        cstr old   = CF("ncg_old%u", c->next++),
             count = CF("ncg_count%u", c->next++);
        OUT("uintptr_t %s=%s?%s->count:0; uintptr_t %s=%s%s;\n",
            old,
            a,
            a,
            count,
            extend ? CF("%s+", old) : "",
            args[0].text);
        OUT("ncg_reserve(%s,%s,sizeof(%s),_Alignof(%s),%s,%u);\n",
            ptr,
            count,
            it,
            it,
            file,
            e->source_line);
        if (!undefined) {
            OUT("if(%s>%s)memset((%s*)%s->data+%s,0,(%s-%s)*sizeof(%s));\n",
                count,
                old,
                it,
                a,
                old,
                count,
                old,
                it);
        }
        OUT("%s->count=%s;\n", a, count);
        if (!cgen_void(c, e->type_index)) {
            result = cgen_temp(c,
                               e->type_index,
                               CF("(%s){(%s*)%s->data+%s,%s-%s}",
                                  cgen_ctype(c, e->type_index),
                                  it,
                                  a,
                                  extend ? old : "0",
                                  count,
                                  extend ? old : "0"));
        }
    }
    array_free(args);
    return result;
}

internal CValue cgen_call(CGen* c, const HirExpr* e)
{
    const HirExpr* callee = &cgen_hir(c)->exprs[e->callee_expr_index];
    if (cgen_kind(c, e->type_index) == STK_Enum) {
        u32 variant = cgen_field_index(c, e->type_index, callee->symbol_handle);
        if (variant != U32_MAX) {
            return cgen_enum(c, e, variant);
        }
    }
    if (callee->kind == HIR_EXPR_Field &&
        callee->operand_expr_index != U32_MAX) {
        u32 bt = cgen_hir(c)->exprs[callee->operand_expr_index].type_index;
        if (cgen_kind(c, bt) == STK_DynamicArray) {
            return cgen_array_call(c, e, callee);
        }
        if (cgen_kind(c, bt) == STK_Box &&
            strcmp(cgen_symbol(c, callee->symbol_handle), "free") == 0) {
            CValue v = cgen_lvalue(c, callee->operand_expr_index);
            OUT("nrt_mem_free(%s);%s=NULL;\n", v.text, v.text);
            return (CValue){"0", e->type_index, false};
        }
    }
    if (cgen_kind(c, e->type_index) == STK_Arena) {
        CValue a       = cgen_temp(c, e->type_index, NULL);
        cstr   args[2] = {"0", "0"};
        for (u32 i = 0; i < e->arg_count && i < 2; ++i) {
            args[i] =
                cgen_expr(c,
                          cgen_hir(c)->call_args[e->first_arg + i].expr_index)
                    .text;
        }
        OUT("nrt_arena_init(&%s,%s,%s,%s,%u);\n",
            a.text,
            args[0],
            args[1],
            cgen_quote(c, e->source_path),
            e->source_line);
        return a;
    }
    u32  fm = U32_MAX, ff = U32_MAX;
    bool direct = cgen_resolve(c,
                               c->module,
                               e->callee_expr_index,
                               cgen_expected(c, callee->type_index),
                               &fm,
                               &ff,
                               0);
    if (!direct && callee->ref_kind == HIR_REF_Decl) {
        direct = cgen_resolve(
            c, c->module, e->callee_expr_index, U32_MAX, &fm, &ff, 0);
    }
    CValue fn =
        direct ? (CValue){cgen_function(c, fm, ff),
                          cgen_translate_type(c,
                                              fm,
                                              c->program->modules[fm]
                                                  .front_end.hir.functions[ff]
                                                  .type_index),
                          false}
               : cgen_expr(c, e->callee_expr_index);
    if (cgen_kind(c, callee->type_index) == STK_Function) {
        fn.type = callee->type_index;
    }
    const HirFunction* function =
        direct ? &c->program->modules[fm].front_end.hir.functions[ff] : NULL;
    const SemaType* ft           = cgen_type(c, fn.type);
    bool            arena_format = false;
    if (function && ft->param_count == 2) {
        u32 pt = cgen_field_type(c, fn.type, 0);
        if (cgen_kind(c, pt) == STK_Pointer &&
            cgen_kind(c, cgen_type(c, pt)->first_param_type) == STK_Arena) {
            const FrontEndState* f = &c->program->modules[fm].front_end;
            if (function->decl_index < array_count(f->sema.decls)) {
                string name = lex_symbol(
                    &f->lexer,
                    f->sema.decls[function->decl_index].symbol_handle);
                cstr n       = CF("%.*s", (int)name.count, name.data);
                arena_format = strstr(n, "_pr") != NULL;
            }
        }
    }
    Array(CValue) args = NULL;
    for (u32 i = 0; i < e->arg_count; ++i) {
        cstr old_arena = c->interpolation_arena;
        if (arena_format && i == 1) {
            c->interpolation_arena = args[0].text;
        }
        u32    ai = cgen_hir(c)->call_args[e->first_arg + i].expr_index;
        u32    at = cgen_hir(c)->exprs[ai].type_index;
        CValue v =
            cgen_kind(c, at) == STK_Array && i < ft->param_count &&
                    cgen_kind(c, cgen_field_type(c, fn.type, i)) == STK_Slice
                ? cgen_lvalue(c, ai)
                : cgen_expr(c, ai);
        if (i < ft->param_count) {
            v = cgen_coerce(c, v, cgen_field_type(c, fn.type, i));
        }
        array_push(args, v);
        c->interpolation_arena = old_arena;
    }
    if (function && array_count(args) < ft->param_count) {
        u32    old_module = c->module, base = array_count(c->overrides);
        string old_path = c->macro_path;
        u32    old_line = c->macro_line;
        c->macro_path   = e->source_path;
        c->macro_line   = e->source_line;
        c->module       = fm;
        for (u32 i = 0; i < ft->param_count; ++i) {
            const HirParam* param =
                &cgen_hir(c)->params[function->first_param + i];
            u32    pt = cgen_field_type(c, function->type_index, i);
            CValue v;
            if (i < array_count(args)) {
                v = (CValue){args[i].text, pt, false};
            } else {
                if (param->default_expr_index == U32_MAX) {
                    cgen_error(
                        c, "missing default argument", e->callee_expr_index);
                    break;
                }
                v = cgen_coerce(c, cgen_expr(c, param->default_expr_index), pt);
                array_push(args, v);
            }
            array_push(c->overrides, ((COverride){fm, param->local_index, v}));
        }
        c->module     = old_module;
        c->macro_path = old_path;
        c->macro_line = old_line;
        if (c->overrides) {
            __array_count(c->overrides) = base;
        }
    }
    Arena temp = {0};
    arena_init(&temp);
    StringBuilder call = {0};
    sb_init(&call, &temp);
    sb_format(&call, "%s(", fn.text);
    for (u32 i = 0; i < array_count(args); ++i) {
        cstr arg = args[i].text;
        if (i < ft->param_count &&
            cgen_kind(c, cgen_field_type(c, fn.type, i)) == STK_VaList &&
            (ft->flags & STF_FunctionCVaList)) {
            arg = CF("nrt_va_native(%s)", arg);
        }
        sb_format(&call, "%s%s", i ? "," : "", arg);
    }
    sb_append_cstr(&call, ")");
    sb_append_null(&call);
    u32    rt     = e->type_index != U32_MAX   ? e->type_index
                    : ft->kind == STK_Function ? ft->return_type
                                               : U32_MAX;
    CValue result = cgen_temp(c, rt, (cstr)sb_to_string(&call).data);
    for (u32 i = 0; i < e->arg_count && i < ft->param_count; ++i) {
        cgen_consume(c,
                     cgen_hir(c)->call_args[e->first_arg + i].expr_index,
                     cgen_field_type(c, fn.type, i));
    }
    array_free(args);
    arena_done(&temp);
    return result;
}

internal void cgen_store(CGen* c, CValue lhs, CValue rhs)
{
    if (cgen_kind(c, lhs.type) == STK_Atomic) {
        OUT("__atomic_store_n(&(%s),%s,__ATOMIC_SEQ_CST);\n",
            lhs.text,
            rhs.text);
    } else {
        OUT("%s=%s;\n",
            lhs.text,
            lhs.bitfield && cgen_kind(c, lhs.type) == STK_Enum
                ? CF("%s.tag", rhs.text)
                : rhs.text);
    }
}
internal bool cgen_integer(CGen* c, u32 t)
{
    switch (cgen_kind(c, t)) {
    case STK_UntypedInteger:
    case STK_I8:
    case STK_I16:
    case STK_I32:
    case STK_I64:
    case STK_Isize:
    case STK_U8:
    case STK_U16:
    case STK_U32:
    case STK_U64:
    case STK_Usize:
        return true;
    default:
        return false;
    }
}
internal CValue cgen_expr(CGen* c, u32 index)
{
    if (index == U32_MAX) {
        return (CValue){"0", U32_MAX, false};
    }
    if (c->failed) {
        return (CValue){"0", U32_MAX, false};
    }
    const HirExpr* e = &cgen_hir(c)->exprs[index];
    u32            t = e->type_index;
    if (t == U32_MAX) {
        t = c->expected_type;
    }
    c->expected_type = U32_MAX;
    if (cgen_kind(c, t) == STK_Function &&
        (e->kind == HIR_EXPR_Index || e->kind == HIR_EXPR_Field ||
         (e->kind == HIR_EXPR_LocalRef && e->ref_kind != HIR_REF_Local))) {
        u32 fm, ff;
        if (cgen_resolve(
                c, c->module, index, cgen_expected(c, t), &fm, &ff, 0)) {
            return cgen_temp(c, t, cgen_function(c, fm, ff));
        }
    }
    if (cgen_kind(c, t) == STK_Atomic &&
        (e->kind == HIR_EXPR_LocalRef || e->kind == HIR_EXPR_Field ||
         e->kind == HIR_EXPR_Index)) {
        CValue v = cgen_lvalue(c, index);
        return cgen_temp(c,
                         cgen_type(c, t)->first_param_type,
                         CF("__atomic_load_n(&(%s),__ATOMIC_SEQ_CST)", v.text));
    }
    switch (e->kind) {
    case HIR_EXPR_DefaultValue:
    case HIR_EXPR_NilLiteral:
        return cgen_temp(c, t, NULL);
    case HIR_EXPR_IntegerLiteral:
        return cgen_temp(c,
                         t,
                         CF("(%s)UINT64_C(%llu)",
                            cgen_ctype(c, t),
                            (unsigned long long)e->integer));
    case HIR_EXPR_FloatLiteral:
        return cgen_temp(c, t, CF("%a", e->floating));
    case HIR_EXPR_BoolLiteral:
        return cgen_temp(c, t, e->boolean ? "true" : "false");
    case HIR_EXPR_StringLiteral:
        {
            if (t == U32_MAX) {
                t = cgen_builtin(c, STK_String);
            }
            string str = cgen_front(c)->lexer.strings[e->string_index];
            cstr   q   = cgen_quote(c, str);
            return cgen_temp(
                c,
                t,
                e->string_is_cstring
                    ? CF("(void*)%s", q)
                    : CF("(NerdString){(uint8_t*)%s,%zu}", q, str.count));
        }
    case HIR_EXPR_BuiltinMacro:
        {
            cstr name = cgen_symbol(c, e->symbol_handle);
            if (strcmp(name, "line") == 0) {
                return cgen_temp(
                    c,
                    t,
                    CF("%u", c->macro_line ? c->macro_line : e->source_line));
            }
            if (strcmp(name, "file") == 0) {
                return cgen_temp(
                    c,
                    t,
                    CF("(NerdString){(uint8_t*)%s,%zu}",
                       cgen_quote(c,
                                  c->macro_path.count ? c->macro_path
                                                      : e->source_path),
                       c->macro_path.count ? c->macro_path.count
                                           : e->source_path.count));
            }
            if (strcmp(name, "embed") == 0) {
                FileMap map  = {0};
                string  data = filemap_load(
                    CF("%.*s", (int)e->source_path.count, e->source_path.data),
                    &map);
                if (!data.data) {
                    cgen_error(c, "cannot read embedded file", index);
                    break;
                }
                cstr  q = cgen_quote(c, data);
                usize n = data.count;
                filemap_unload(&map);
                return cgen_temp(
                    c, t, CF("(%s){(void*)%s,%zu}", cgen_ctype(c, t), q, n));
            }
            break;
        }
    case HIR_EXPR_FunctionRef:
        return cgen_temp(c, t, cgen_function(c, c->module, e->ref_index));
    case HIR_EXPR_LocalRef:
        {
            u32 bi = e->ref_kind == HIR_REF_Binding ? e->ref_index
                     : e->ref_kind == HIR_REF_Decl &&
                             e->ref_index <
                                 array_count(cgen_hir(c)->decl_binding_indices)
                         ? cgen_hir(c)->decl_binding_indices[e->ref_index]
                         : U32_MAX;
            if (bi < array_count(cgen_hir(c)->bindings)) {
                const HirBinding* binding = &cgen_hir(c)->bindings[bi];
                if (binding->kind == HIR_BINDING_Value) {
                    const HirValue* v =
                        &cgen_hir(c)->values[binding->target_index];
                    if (v->kind == HIR_VALUE_Constant &&
                        v->decl_index < array_count(cgen_sema(c)->decls) &&
                        cgen_sema(c)
                                ->decls[v->decl_index]
                                .import_module_index == U32_MAX &&
                        v->value_expr_index != index) {
                        if (++c->constant_depth > 64) {
                            cgen_error(c, "cyclic constant", index);
                            --c->constant_depth;
                            return (CValue){"0", t, false};
                        }
                        CValue result =
                            cgen_coerce(c,
                                        cgen_expr(c, v->value_expr_index),
                                        v->type_index);
                        --c->constant_depth;
                        return result;
                    }
                }
            }

            if (e->ref_kind == HIR_REF_None && cgen_kind(c, t) == STK_Enum) {
                u32 v = cgen_field_index(c, t, e->symbol_handle);
                if (v != U32_MAX) {
                    return cgen_enum(c, e, v);
                }
            }
            {
                CValue v = cgen_ref(c, e);
                return cgen_temp(c, v.type, v.text);
            }
        }
    case HIR_EXPR_Unary:
        {
            if (e->unary_op == HIR_UNARY_AddressOf) {
                CValue v = cgen_lvalue(c, e->operand_expr_index);
                return cgen_temp(c, t, CF("&(%s)", v.text));
            }
            if (e->unary_op == HIR_UNARY_Deref) {
                CValue v = cgen_lvalue(c, index);
                return cgen_temp(c, t, v.text);
            }
            CValue v = cgen_expr(c, e->operand_expr_index);
            if (e->unary_op == HIR_UNARY_Negate && cgen_integer(c, t)) {
                return cgen_temp(c,
                                 t,
                                 CF("(%s)(UINT64_C(0)-(uint64_t)%s)",
                                    cgen_ctype(c, t),
                                    v.text));
            }
            return cgen_temp(c,
                             t,
                             CF("%s(%s)",
                                e->unary_op == HIR_UNARY_Negate       ? "-"
                                : e->unary_op == HIR_UNARY_BitwiseNot ? "~"
                                                                      : "!",
                                e->unary_op == HIR_UNARY_LogicalNot
                                    ? cgen_truth(c, v)
                                    : v.text));
        }
    case HIR_EXPR_Binary:
        {
            CValue a = cgen_expr(c, e->lhs_expr_index);
            if (e->binary_op == HIR_BINARY_LogicalAnd ||
                e->binary_op == HIR_BINARY_LogicalOr) {
                CValue r = cgen_temp(c, t, cgen_truth(c, a));
                OUT("if(%s%s) {\n",
                    e->binary_op == HIR_BINARY_LogicalOr ? "!" : "",
                    r.text);
                CValue b = cgen_expr(c, e->rhs_expr_index);
                OUT("%s=%s; }\n", r.text, cgen_truth(c, b));
                return r;
            }
            if (e->binary_op == HIR_BINARY_InRange) {
                const HirExpr* re = &cgen_hir(c)->exprs[e->rhs_expr_index];
                CValue         lo = cgen_expr(c, re->lhs_expr_index);
                CValue result = cgen_temp(c, t, CF("%s>=%s", a.text, lo.text));
                OUT("if(%s) {\n", result.text);
                CValue hi = cgen_expr(c, re->rhs_expr_index);
                OUT("%s=%s%s%s; }\n",
                    result.text,
                    a.text,
                    re->kind == HIR_EXPR_RangeInclusive ? "<=" : "<",
                    hi.text);
                return result;
            }
            CValue b = cgen_expr(c, e->rhs_expr_index);
            if (e->binary_op == HIR_BINARY_Equal ||
                e->binary_op == HIR_BINARY_NotEqual) {
                bool nil = cgen_hir(c)->exprs[e->lhs_expr_index].kind ==
                               HIR_EXPR_NilLiteral ||
                           cgen_hir(c)->exprs[e->rhs_expr_index].kind ==
                               HIR_EXPR_NilLiteral;
                if (nil && (cgen_kind(c, a.type) == STK_Box ||
                            cgen_kind(c, a.type) == STK_DynamicArray)) {
                    return cgen_temp(
                        c,
                        t,
                        CF("%s %s %s",
                           a.text,
                           e->binary_op == HIR_BINARY_Equal ? "==" : "!=",
                           b.text));
                }
                if (nil && cgen_kind(c, a.type) == STK_Slice) {
                    return cgen_temp(
                        c,
                        t,
                        CF("%s(%s.data==%s.data && %s.count==%s.count)",
                           e->binary_op == HIR_BINARY_Equal ? "" : "!",
                           a.text,
                           b.text,
                           a.text,
                           b.text));
                }

                if (cgen_kind(c, a.type) == STK_UntypedInteger) {
                    a = cgen_coerce(c, a, b.type);
                } else {
                    b = cgen_coerce(c, b, a.type);
                }
                return cgen_temp(
                    c,
                    t,
                    CF("%s(%s)",
                       e->binary_op == HIR_BINARY_NotEqual ? "!" : "",
                       cgen_equal(c, a, b, 0)));
            }
            if ((e->binary_op == HIR_BINARY_Add ||
                 e->binary_op == HIR_BINARY_Subtract) &&
                (cgen_kind(c, a.type) == STK_Pointer ||
                 cgen_kind(c, b.type) == STK_Pointer)) {
                CValue pointer = cgen_kind(c, a.type) == STK_Pointer ? a : b,
                       other   = cgen_kind(c, a.type) == STK_Pointer ? b : a;
                cstr   it =
                    cgen_ctype(c, cgen_type(c, pointer.type)->first_param_type);
                if (strcmp(it, "void") == 0) {
                    it = "uint8_t";
                }
                cstr rhs = cgen_kind(c, other.type) == STK_Pointer
                               ? CF("(%s*)%s", it, other.text)
                               : other.text;
                return cgen_temp(c,
                                 t,
                                 CF("((%s*)%s %s %s)",
                                    it,
                                    pointer.text,
                                    e->binary_op == HIR_BINARY_Add ? "+" : "-",
                                    rhs));
            }
            static cstr ops[] = {"+",
                                 "-",
                                 "*",
                                 "/",
                                 "%",
                                 "&",
                                 "^",
                                 "|",
                                 "<<",
                                 ">>",
                                 "==",
                                 "!=",
                                 "<",
                                 "<=",
                                 ">",
                                 ">="};
            if (e->binary_op < HIR_BINARY_Equal) {
                a = cgen_coerce(c, a, t);
                b = cgen_coerce(c, b, t);
            }
            if (cgen_integer(c, t) && (e->binary_op == HIR_BINARY_Add ||
                                       e->binary_op == HIR_BINARY_Subtract ||
                                       e->binary_op == HIR_BINARY_Multiply ||
                                       e->binary_op == HIR_BINARY_ShiftLeft)) {
                return cgen_temp(c,
                                 t,
                                 CF("(%s)((uint64_t)%s %s (uint64_t)%s)",
                                    cgen_ctype(c, t),
                                    a.text,
                                    ops[e->binary_op],
                                    b.text));
            }
            return cgen_temp(
                c, t, CF("(%s %s %s)", a.text, ops[e->binary_op], b.text));
        }
    case HIR_EXPR_Atomic:
        {
            CValue receiver = cgen_expr(c, e->operand_expr_index);
            u32    rt       = cgen_type(c, receiver.type)->first_param_type;
            if (cgen_kind(c, rt) == STK_Atomic) {
                rt = cgen_type(c, rt)->first_param_type;
            }
            cstr ptr = CF("(%s*)%s", cgen_ctype(c, rt), receiver.text);
            static const int orders[] = {0, 2, 3, 4, 5};
            int              order    = e->atomic_op == HIR_ATOMIC_Load
                                            ? (e->atomic_order == 0   ? 0
                                               : e->atomic_order == 1 ? 2
                                                                      : 5)
                                        : e->atomic_op == HIR_ATOMIC_Store
                                            ? (e->atomic_order == 0   ? 0
                                               : e->atomic_order == 1 ? 3
                                                                      : 5)
                                            : orders[e->atomic_order];
            if (e->atomic_op == HIR_ATOMIC_Load) {
                return cgen_temp(
                    c, t, CF("__atomic_load_n(%s,%d)", ptr, order));
            }
            if (e->atomic_op == HIR_ATOMIC_CompareExchange ||
                e->atomic_op == HIR_ATOMIC_CompareExchangeWeak) {
                CValue expected =
                           cgen_coerce(c, cgen_expr(c, e->lhs_expr_index), rt),
                       desired =
                           cgen_coerce(c, cgen_expr(c, e->rhs_expr_index), rt);
                CValue success = cgen_temp(
                    c,
                    cgen_builtin(c, STK_Bool),
                    CF("__atomic_compare_exchange_n(%s,&%s,%s,%d,%d,%d)",
                       ptr,
                       expected.text,
                       desired.text,
                       e->atomic_op == HIR_ATOMIC_CompareExchangeWeak,
                       order,
                       e->atomic_failure_order == 0   ? 0
                       : e->atomic_failure_order == 1 ? 2
                                                      : 5));
                CValue result = cgen_temp(c, t, NULL);
                OUT("%s.tag=%s?0:1;\n", result.text, success.text);
                if (cgen_type(c, t)->param_count > 1 &&
                    !cgen_void(c, cgen_field_type(c, t, 1))) {
                    OUT("%s.payload.f1=%s;\n", result.text, expected.text);
                }
                return result;
            }
            CValue v = cgen_coerce(c, cgen_expr(c, e->extra_expr_index), rt);
            static cstr ops[] = {"load",
                                 "store",
                                 "exchange",
                                 "fetch_add",
                                 "fetch_sub",
                                 "fetch_and",
                                 "fetch_or",
                                 "fetch_xor"};
            return cgen_temp(c,
                             t,
                             CF("__atomic_%s%s(%s,%s,%d)",
                                ops[e->atomic_op],
                                e->atomic_op <= HIR_ATOMIC_Exchange ? "_n" : "",
                                ptr,
                                v.text,
                                order));
        }
    case HIR_EXPR_Assign:
        {
            CValue         lhs    = cgen_lvalue(c, e->lhs_expr_index);
            const HirExpr* update = &cgen_hir(c)->exprs[e->rhs_expr_index];
            const HirExpr* target = &cgen_hir(c)->exprs[e->lhs_expr_index];
            if (cgen_kind(c, lhs.type) == STK_Atomic &&
                update->kind == HIR_EXPR_Binary &&
                target->kind == HIR_EXPR_LocalRef) {
                const HirExpr* previous =
                    &cgen_hir(c)->exprs[update->lhs_expr_index];
                cstr op = update->binary_op == HIR_BINARY_Add          ? "add"
                          : update->binary_op == HIR_BINARY_Subtract   ? "sub"
                          : update->binary_op == HIR_BINARY_BitwiseAnd ? "and"
                          : update->binary_op == HIR_BINARY_BitwiseOr  ? "or"
                          : update->binary_op == HIR_BINARY_BitwiseXor ? "xor"
                                                                       : NULL;
                if (op && previous->kind == HIR_EXPR_LocalRef &&
                    previous->ref_kind == target->ref_kind &&
                    previous->ref_index == target->ref_index) {
                    CValue arg = cgen_expr(c, update->rhs_expr_index);
                    return cgen_temp(
                        c,
                        cgen_type(c, lhs.type)->first_param_type,
                        CF("__atomic_fetch_%s(&(%s),%s,__ATOMIC_SEQ_CST)",
                           op,
                           lhs.text,
                           arg.text));
                }
            }
            CValue rhs =
                cgen_coerce(c, cgen_expr(c, e->rhs_expr_index), lhs.type);
            cgen_store(c, lhs, rhs);
            return rhs;
        }
    case HIR_EXPR_Cast:
        {
            CValue v = cgen_expr(c, e->operand_expr_index);
            if (cgen_kind(c, t) == STK_Slice &&
                cgen_kind(c, v.type) == STK_Pointer &&
                e->extra_expr_index != U32_MAX) {
                CValue count = cgen_expr(c, e->extra_expr_index);
                return cgen_temp(
                    c,
                    t,
                    CF("(%s){%s,%s}", cgen_ctype(c, t), v.text, count.text));
            }
            return cgen_coerce(c, v, t);
        }
    case HIR_EXPR_Index:
        {
            CValue v = cgen_lvalue(c, index);
            return cgen_temp(c, t, v.text);
        }
    case HIR_EXPR_Tuple:
    case HIR_EXPR_Array:
    case HIR_EXPR_Plex:
    case HIR_EXPR_PlexUpdate:
        {
            CValue       r    = e->kind == HIR_EXPR_PlexUpdate
                                    ? cgen_expr(c, e->operand_expr_index)
                                    : cgen_temp(c, t, NULL);
            SemaTypeKind k    = cgen_kind(c, t);
            cstr         data = NULL;
            if (k == STK_DynamicArray) {
                cstr capacity =
                    e->extra_expr_index != U32_MAX
                        ? cgen_expr(c, e->extra_expr_index).text
                        : CF("%u",
                             e->integer > e->arg_count ? (u32)e->integer
                                                       : e->arg_count);
                OUT("ncg_reserve(&%s,%s,sizeof(%s),_Alignof(%s),%s,%u); "
                    "%s->count=%u;\n",
                    r.text,
                    capacity,
                    cgen_ctype(c, cgen_type(c, t)->first_param_type),
                    cgen_ctype(c, cgen_type(c, t)->first_param_type),
                    cgen_quote(c, e->source_path),
                    e->source_line,
                    r.text,
                    e->arg_count);
            }
            if (k == STK_Slice) {
                data = (c->global_init || c->returning)
                           ? CF("ncg_m%u_array%u", c->module, index)
                           : CF("ncg_data%u", c->next++);
                if (!c->global_init && !c->returning) {
                    OUT("%s %s[%u]={0};\n",
                        cgen_ctype(c, cgen_type(c, t)->first_param_type),
                        data,
                        e->arg_count ? e->arg_count : 1);
                }
                OUT("%s.data=%s; %s.count=%u;\n",
                    r.text,
                    data,
                    r.text,
                    e->arg_count);
            }
            for (u32 i = 0; i < e->arg_count; ++i) {
                const HirCallArg* arg =
                    &cgen_hir(c)->call_args[e->first_arg + i];
                u32 fi = (k == STK_Plex || k == STK_Union)
                             ? cgen_field_index(c, t, arg->symbol_handle)
                             : i;
                u32 it =
                    (k == STK_Array || k == STK_Slice || k == STK_DynamicArray)
                        ? cgen_type(c, t)->first_param_type
                        : cgen_field_type(c, t, fi);
                CValue v = cgen_expr_as(c, arg->expr_index, it);
                cgen_consume(c, arg->expr_index, it);
                if (k == STK_DynamicArray) {
                    OUT("((%s*)%s->data)[%u]=%s;\n",
                        cgen_ctype(c, it),
                        r.text,
                        i,
                        v.text);
                } else if (k == STK_Slice) {
                    OUT("%s[%u]=%s;\n", data, i, v.text);
                } else if (k == STK_Array) {
                    OUT("%s.data[%u]=%s;\n", r.text, i, v.text);
                } else {
                    OUT("%s.f%u=%s;\n",
                        r.text,
                        fi,
                        cgen_bitfield(c, t, fi) && cgen_kind(c, it) == STK_Enum
                            ? CF("%s.tag", v.text)
                            : v.text);
                }
            }
            return r;
        }
    case HIR_EXPR_TupleField:
    case HIR_EXPR_Field:
        {
            if (e->ref_kind == HIR_REF_Binding || e->ref_kind == HIR_REF_Decl) {
                CValue v = cgen_ref(c, e);
                return cgen_temp(c, t, v.text);
            }
            u32 owner = cgen_hir(c)->exprs[e->operand_expr_index].type_index;
            if (cgen_kind(c, owner) == STK_Module) {
                u32                  module = cgen_type(c, owner)->return_type;
                const FrontEndState* f = &c->program->modules[module].front_end;
                cstr                 name = cgen_symbol(c, e->symbol_handle);
                for (u32 i = 0; i < array_count(f->hir.bindings); ++i) {
                    string other =
                        lex_symbol(&f->lexer, f->hir.bindings[i].symbol_handle);
                    if (string_eq_cstr(other, name)) {
                        return cgen_temp(c, t, cgen_binding(c, module, i, 0));
                    }
                }
            }
            if (cgen_kind(c, t) == STK_Enum) {
                u32 v = cgen_field_index(c, t, e->symbol_handle);
                if (v != U32_MAX) {
                    return cgen_enum(c, e, v);
                }
            }
            cstr name = cgen_symbol(c, e->symbol_handle);
            u32  bt   = cgen_hir(c)->exprs[e->operand_expr_index].type_index;
            if (strcmp(name, "size") == 0 &&
                cgen_field_index(c, bt, e->symbol_handle) == U32_MAX) {
                return cgen_temp(
                    c,
                    t,
                    CF("%s",
                       cgen_kind(c, bt) == STK_Nil || cgen_void(c, bt) ? "0"
                       : cgen_kind(c, bt) == STK_UntypedInteger
                           ? "4"
                           : CF("sizeof(%s)", cgen_ctype(c, bt))));
            }
            if (cgen_kind(c, bt) == STK_Box &&
                (strcmp(name, "data") == 0 || strcmp(name, "count") == 0 ||
                 strcmp(name, "bytes") == 0)) {
                CValue v = cgen_expr(c, e->operand_expr_index);
                if (strcmp(name, "data") == 0) {
                    return cgen_temp(c, t, v.text);
                }
                return cgen_temp(
                    c,
                    t,
                    CF("(%s?nrt_mem_size(%s)%s:0)",
                       v.text,
                       v.text,
                       strcmp(name, "bytes") == 0
                           ? ""
                           : CF("/sizeof(%s)",
                                cgen_ctype(
                                    c, cgen_type(c, bt)->first_param_type))));
            }
            CValue       v = cgen_kind(c, bt) == STK_Array
                                 ? cgen_lvalue(c, e->operand_expr_index)
                                 : cgen_expr(c, e->operand_expr_index);
            SemaTypeKind k = cgen_kind(c, bt);
            while (k == STK_Pointer || k == STK_Box) {
                bt     = cgen_type(c, bt)->first_param_type;
                v.text = CF("(*(%s*)%s)", cgen_ctype(c, bt), v.text);
                k      = cgen_kind(c, bt);
            }
            if (k == STK_Array) {
                if (strcmp(name, "count") == 0) {
                    return cgen_temp(
                        c, t, CF("%u", cgen_type(c, bt)->return_type));
                }
                if (strcmp(name, "bytes") == 0) {
                    return cgen_temp(c, t, CF("sizeof(%s)", v.text));
                }
                return cgen_temp(c, t, CF("%s.data", v.text));
            }
            if (k == STK_Slice || k == STK_String) {
                if (strcmp(name, "bytes") == 0) {
                    return cgen_temp(
                        c,
                        t,
                        k == STK_String
                            ? CF("%s.count", v.text)
                            : CF("%s.count*sizeof(%s)",
                                 v.text,
                                 cgen_ctype(
                                     c, cgen_type(c, bt)->first_param_type)));
                }
                return cgen_temp(c, t, CF("%s.%s", v.text, name));
            }
            if (k == STK_DynamicArray) {
                if (strcmp(name, "bytes") == 0) {
                    return cgen_temp(
                        c,
                        t,
                        CF("(%s?%s->count*sizeof(%s):0)",
                           v.text,
                           v.text,
                           cgen_ctype(c, cgen_type(c, bt)->first_param_type)));
                }
                return cgen_temp(
                    c, t, CF("(%s?%s->%s:0)", v.text, v.text, name));
            }
            u32 fi = e->kind == HIR_EXPR_TupleField
                         ? (u32)e->integer
                         : cgen_field_index(c, bt, e->symbol_handle);
            if (fi == U32_MAX) {
                break;
            }
            return cgen_temp(
                c,
                t,
                cgen_bitfield(c, bt, fi) && cgen_kind(c, t) == STK_Enum
                    ? CF("(%s){.tag=%s.f%u}", cgen_ctype(c, t), v.text, fi)
                    : CF("%s.f%u", v.text, fi));
        }
    case HIR_EXPR_Slice:
        {
            u32    bt = cgen_hir(c)->exprs[e->operand_expr_index].type_index;
            CValue v  = cgen_kind(c, bt) == STK_Array
                            ? cgen_lvalue(c, e->operand_expr_index)
                            : cgen_expr(c, e->operand_expr_index);
            cstr   lo = e->lhs_expr_index == U32_MAX
                            ? "0"
                            : cgen_expr(c, e->lhs_expr_index).text;
            cstr hi = e->rhs_expr_index == U32_MAX
                          ? (cgen_kind(c, bt) == STK_Array
                                 ? CF("%u", cgen_type(c, bt)->return_type)
                                 : (cgen_kind(c, bt) == STK_DynamicArray
                                        ? CF("(%s?%s->count:0)", v.text, v.text)
                                        : CF("%s.count", v.text)))
                          : cgen_expr(c, e->rhs_expr_index).text;
            cstr it = cgen_kind(c, bt) == STK_String
                          ? "uint8_t"
                          : cgen_ctype(c, cgen_type(c, bt)->first_param_type);
            cstr data = cgen_kind(c, bt) == STK_Pointer ? v.text
                        : cgen_kind(c, bt) == STK_DynamicArray
                            ? CF("(%s?%s->data:NULL)", v.text, v.text)
                            : CF("%s.data", v.text);
            return cgen_temp(c,
                             t,
                             CF("(%s){((%s*)%s)+%s, (%s>%s?%s-%s:0)}",
                                cgen_ctype(c, t),
                                it,
                                data,
                                lo,
                                hi,
                                lo,
                                hi,
                                lo));
        }
    case HIR_EXPR_StringConcat:
    case HIR_EXPR_InterpolatedString:
        {
            cstr mark = CF("ncg_mark%u", c->next++);
            OUT("uintptr_t %s=nrt_string_builder_mark();\n", mark);
            u32 n = e->kind == HIR_EXPR_StringConcat ? 2 : e->arg_count;
            for (u32 i = 0; i < n; ++i) {
                u32 ei =
                    e->kind == HIR_EXPR_StringConcat
                        ? (i ? e->rhs_expr_index : e->lhs_expr_index)
                        : cgen_hir(c)->call_args[e->first_arg + i].expr_index;
                CValue v = cgen_expr(c, ei);
                cgen_display(c, v, 0);
            }
            CValue r = cgen_temp(c, t, NULL);
            if (c->interpolation_arena) {
                OUT("nrt_string_builder_finish_in(&%s,%s,%s);\n",
                    r.text,
                    mark,
                    c->interpolation_arena);
            } else {
                OUT("nrt_string_builder_finish(&%s,%s);\n", r.text, mark);
            }
            return r;
        }
    case HIR_EXPR_Block:
    case HIR_EXPR_On:
    case HIR_EXPR_For:
        return cgen_control(c, e);
    case HIR_EXPR_Call:
        return cgen_call(c, e);
    case HIR_EXPR_Propagate:
        {
            CValue v        = cgen_expr(c, e->operand_expr_index);
            bool   optional = cgen_type(c, v.type)->flags & STF_Optional;
            u32    success  = optional ? 1 : 0;
            OUT("if(%s.tag != %u) {\n", v.text, success);
            CValue failure = cgen_temp(c, c->return_type, NULL);
            if (!optional) {
                OUT("%s.tag=1;\n", failure.text);
                CValue payload = {CF("%s.payload.f1", v.text),
                                  cgen_field_type(c, v.type, 1),
                                  false};
                payload        = cgen_coerce(
                    c, payload, cgen_field_type(c, c->return_type, 1));
                OUT("%s.payload.f1=%s;\n", failure.text, payload.text);
            }
            cgen_return(c, failure);
            OUT("}\n");
            return cgen_void(c, t)
                       ? (CValue){"0", t, false}
                       : cgen_temp(c, t, CF("%s.payload.f%u", v.text, success));
        }
    case HIR_EXPR_Box:
        {
            u32    it  = cgen_type(c, t)->first_param_type;
            CValue arg = {"1", cgen_builtin(c, STK_Usize), false};
            if (e->arg_count) {
                arg = cgen_expr(
                    c, cgen_hir(c)->call_args[e->first_arg].expr_index);
            }
            if (cgen_kind(c, arg.type) == STK_Pointer ||
                cgen_kind(c, arg.type) == STK_Box) {
                return cgen_coerce(c, arg, t);
            }
            CValue r = cgen_temp(c,
                                 t,
                                 CF("nrt_mem_alloc(sizeof(%s)*(%s),16,%s,%u)",
                                    cgen_ctype(c, it),
                                    arg.text,
                                    cgen_quote(c, e->source_path),
                                    e->source_line));
            if (!e->arg_count) {
                OUT("*(%s*)%s=%s;\n",
                    cgen_ctype(c, it),
                    r.text,
                    cgen_zero(c, it));
            }
            return r;
        }
    case HIR_EXPR_VaCopy:
        {
            CValue v = cgen_expr(c, e->operand_expr_index);
            return cgen_temp(c, t, CF("nrt_va_copy(%s)", v.text));
        }
    case HIR_EXPR_VaNext:
        {
            CValue v = cgen_expr(c, e->operand_expr_index);
            return cgen_temp(
                c, t, CF("va_arg(%s->args,%s)", v.text, cgen_ctype(c, t)));
        }
    case HIR_EXPR_VaFormat:
        {
            CValue v   = cgen_expr(c, e->operand_expr_index),
                   fmt = cgen_expr(c, e->extra_expr_index),
                   r   = cgen_temp(c, t, NULL);
            OUT("nrt_va_format(&%s,%s,%s);\n", r.text, v.text, fmt.text);
            return r;
        }
    default:
        break;
    }
    cgen_error(c, CF("unsupported expression kind %u", e->kind), index);
    return (CValue){"0", t, false};
}

internal void cgen_stmt(CGen* c, u32 index)
{
    if (c->failed) {
        return;
    }
    const HirStmt* st = &cgen_hir(c)->stmts[index];
    switch (st->kind) {
    case HIR_STMT_Expr:
        if (st->expr_index != U32_MAX &&
            cgen_hir(c)->exprs[st->expr_index].kind != HIR_EXPR_Unsupported) {
            cgen_expr(c, st->expr_index);
        }
        break;
    case HIR_STMT_Let:
        {
            if (st->local_index == U32_MAX) {
                if (st->expr_index != U32_MAX &&
                    cgen_hir(c)->exprs[st->expr_index].kind !=
                        HIR_EXPR_Unsupported) {
                    cgen_expr(c, st->expr_index);
                }
                break;
            }
            cgen_local(c, st->local_index, st->type_index);
            CValue v = st->expr_index == U32_MAX ||
                               cgen_hir(c)->exprs[st->expr_index].kind ==
                                   HIR_EXPR_Unsupported
                           ? cgen_temp(c, st->type_index, NULL)
                           : cgen_coerce(c,
                                         cgen_expr(c, st->expr_index),
                                         st->type_index);
            OUT("ncg_l%u=%s;\n", st->local_index, v.text);
            cgen_consume(c, st->expr_index, st->type_index);
            if (cgen_kind(c, st->type_index) == STK_Box) {
                array_push(c->cleanups, ((CCleanup){false, st->local_index}));
            }
            break;
        }
    case HIR_STMT_Assign:
        {
            CValue lhs = cgen_lvalue(c, st->target_expr_index);
            CValue v   = cgen_coerce(c, cgen_expr(c, st->expr_index), lhs.type);
            cgen_store(c, lhs, v);
            cgen_consume(c, st->expr_index, lhs.type);
            break;
        }
    case HIR_STMT_DestructureLet:
    case HIR_STMT_DestructureAssign:
        {
            CValue v = cgen_expr(c, st->expr_index);
            for (u32 i = 0; i < st->body_block_index; ++i) {
                const HirDestructureItem* item =
                    &cgen_hir(c)->destructure_items[st->target_expr_index + i];
                cgen_local(c, item->local_index, item->type_index);
                OUT("ncg_l%u=%s.f%u;\n",
                    item->local_index,
                    v.text,
                    item->field_index);
            }
            break;
        }
    case HIR_STMT_Return:
        {
            bool old     = c->returning;
            c->returning = true;
            CValue v     = cgen_expr_as(c, st->expr_index, c->return_type);
            c->returning = old;
            cgen_consume(c, st->expr_index, c->return_type);
            cgen_return(c, v);
            break;
        }
    case HIR_STMT_Assert:
        {
            CValue condition = cgen_expr(c, st->expr_index);
            CValue message =
                st->target_expr_index != U32_MAX
                    ? cgen_expr(c, st->target_expr_index)
                    : (CValue){
                          "(NerdString){(uint8_t*)\"assertion failed\",16}",
                          U32_MAX,
                          false};
            OUT("nrt_nerd_assert(%s,%s,%u,&(%s));\n",
                cgen_truth(c, condition),
                cgen_quote(c, st->source_path),
                st->source_line,
                message.text);
            break;
        }
    case HIR_STMT_Defer:
        array_push(c->cleanups, ((CCleanup){true, st->body_block_index}));
        break;
    case HIR_STMT_Block:
        cgen_block(c, st->body_block_index);
        break;
    case HIR_STMT_Break:
    case HIR_STMT_Continue:
        {
            CTarget* target = NULL;
            for (u32 i = array_count(c->targets); i > 0; --i) {
                CTarget* p = &c->targets[i - 1];
                if ((st->symbol_handle == U32_MAX ||
                     st->symbol_handle == p->symbol) &&
                    (st->kind == HIR_STMT_Break || p->loop)) {
                    target = p;
                    break;
                }
            }
            if (!target) {
                cgen_error(c, "missing control target", index);
                break;
            }
            CTarget saved = *target;
            if (st->expr_index != U32_MAX && saved.result) {
                CValue v =
                    cgen_coerce(c, cgen_expr(c, st->expr_index), saved.type);
                OUT("%s=%s;\n", saved.result, v.text);
            }
            cgen_cleanup_to(c, saved.cleanup);
            OUT("goto ncg_%s%u;\n",
                st->kind == HIR_STMT_Break ? "end" : "continue",
                saved.id);
            break;
        }
    }
}
internal void cgen_value_block(CGen* c, u32 index, CValue result)
{
    if (index == U32_MAX) {
        return;
    }
    u32             base  = array_count(c->cleanups);
    const HirBlock* block = &cgen_hir(c)->blocks[index];
    OUT("{\n");
    // Local function bindings are visible throughout their lexical block.
    for (u32 i = 0; i < block->stmt_count; ++i) {
        const HirStmt* st = &cgen_hir(c)->stmts[block->stmt_indices[i]];
        if (st->kind == HIR_STMT_Let && st->local_index != U32_MAX &&
            st->expr_index != U32_MAX &&
            cgen_hir(c)->exprs[st->expr_index].kind == HIR_EXPR_FunctionRef) {
            cgen_local(c, st->local_index, st->type_index);
            OUT("ncg_l%u=%s;\n",
                st->local_index,
                cgen_function(c,
                              c->module,
                              cgen_hir(c)->exprs[st->expr_index].ref_index));
        }
    }
    for (u32 i = 0; i < block->stmt_count; ++i) {
        u32            si = block->stmt_indices[i];
        const HirStmt* st = &cgen_hir(c)->stmts[si];
        if (i + 1 == block->stmt_count && st->kind == HIR_STMT_Expr &&
            !cgen_void(c, result.type)) {
            CValue v =
                cgen_coerce(c, cgen_expr(c, st->expr_index), result.type);
            OUT("%s=%s;\n", result.text, v.text);
        } else {
            cgen_stmt(c, si);
        }
    }
    cgen_cleanup_to(c, base);
    if (c->cleanups) {
        __array_count(c->cleanups) = base;
    }
    OUT("}\n");
}
internal void cgen_block(CGen* c, u32 index)
{
    cgen_value_block(c, index, (CValue){NULL, U32_MAX, false});
}

internal void cgen_signature(CGen* c, StringBuilder* b, u32 fi, bool prototype)
{
    const HirFunction* fn = &cgen_hir(c)->functions[fi];
    const SemaType*    t  = cgen_type(c, fn->type_index);
    sb_format(b,
              "%s%s %s(",
              fn->kind == HIR_FUNCTION_Ffi ? "extern " : "static ",
              cgen_ctype(c, t->return_type),
              cgen_function(c, c->module, fi));
    for (u32 i = 0; i < t->param_count; ++i) {
        u32 local = i < fn->param_count
                        ? cgen_hir(c)->params[fn->first_param + i].local_index
                        : i;
        sb_format(b,
                  "%s%s ncg_l%u",
                  i ? ", " : "",
                  cgen_ctype(c, cgen_field_type(c, fn->type_index, i)),
                  (local == U32_MAX ? i : local));
    }
    if (t->flags & STF_FunctionVarargs) {
        sb_format(b, "%s...", t->param_count ? ", " : "");
    } else if (!t->param_count) {
        sb_append_cstr(b, "void");
    }
    sb_append_char(b, ')');
    if (prototype) {
        if (fn->kind == HIR_FUNCTION_Ffi) {
            sb_format(b,
                      " __asm__(%s)",
                      cgen_quote(c,
                                 lex_symbol(&cgen_front(c)->lexer,
                                            fn->ffi_symbol_handle)));
        }
        sb_append_cstr(b, ";\n");
    }
}
internal bool cgen_imported_value(CGen* c, const HirValue* v)
{
    return v->decl_index < array_count(cgen_sema(c)->decls) &&
           cgen_sema(c)->decls[v->decl_index].import_module_index != U32_MAX;
}
internal void cgen_reset_body(CGen* c)
{
    arena_reset(&c->body_arena);
    arena_reset(&c->decl_arena);
    sb_init(&c->body, &c->body_arena);
    sb_init(&c->declarations, &c->decl_arena);
    array_free(c->locals);
    array_free(c->local_types);
    array_free(c->cleanups);
    array_free(c->targets);
    c->va_local      = U32_MAX;
    c->global_init   = false;
    c->expected_type = U32_MAX;
}
internal void cgen_init_module(CGen* c, u32 module, Array(bool) * done)
{
    if ((*done)[module]) {
        return;
    }
    (*done)[module]     = true;
    const ModuleInfo* m = &c->program->modules[module];
    for (u32 i = 0; i < array_count(m->imported_module_indices); ++i) {
        cgen_init_module(c, m->imported_module_indices[i], done);
    }
    for (u32 i = 0; i < array_count(m->front_end.hir.imports); ++i) {
        cgen_init_module(c, m->front_end.hir.imports[i].module_index, done);
    }
    for (u32 i = 0; i < array_count(m->front_end.hir.module_imports); ++i) {
        cgen_init_module(
            c, m->front_end.hir.module_imports[i].module_index, done);
    }
    sb_format(&c->out, "ncg_init%u();\n", module);
}
bool cgen_save_program(const ProgramInfo*        program,
                       const NerdArtifactConfig* artifacts)
{
    CGen  state = {.program = program, .va_local = U32_MAX};
    CGen* c     = &state;
    arena_init(&c->arena);
    arena_init(&c->out_arena);
    arena_init(&c->body_arena);
    arena_init(&c->decl_arena);
    sb_init(&c->out, &c->out_arena);
    sb_append_cstr(&c->out,
                   "/* Generated from Nerd HIR. Compile with clang -std=gnu11 "
                   "source.c -o program (plus external libraries). */\n");
    if (artifacts->release) {
        sb_append_cstr(&c->out, "#define NDEBUG 1\n");
    }
    sb_append_string(&c->out,
                     string_from((u8*)cgen_runtime, sizeof(cgen_runtime)));
    sb_append_string(&c->out,
                     string_from((u8*)cgen_helpers, sizeof(cgen_helpers)));
    for (u32 m = 0; m < array_count(program->modules); ++m) {
        array_push(c->offsets, (u32)array_count(c->type_map));
        for (u32 i = 0;
             i < array_count(program->modules[m].front_end.sema.types);
             ++i) {
            array_push(c->type_map, U32_MAX);
        }
    }
    for (c->module = 0; c->module < array_count(program->modules);
         ++c->module) {
        for (u32 i = 0; i < array_count(cgen_sema(c)->types); ++i) {
            cgen_canonical(c, i);
        }
    }
    for (u32 i = 0; i < array_count(c->types); ++i) {
        cgen_define_type(c, i);
    }
    for (c->module = 0; c->module < array_count(program->modules);
         ++c->module) {
        const Hir* h = cgen_hir(c);
        for (u32 i = 0; i < array_count(h->functions); ++i) {
            cgen_signature(c, &c->out, i, true);
        }
        for (u32 i = 0; i < array_count(h->exprs); ++i) {
            const HirExpr* e = &h->exprs[i];
            if (e->kind == HIR_EXPR_Array &&
                cgen_kind(c, e->type_index) == STK_Slice) {
                sb_format(&c->out,
                          "static %s ncg_m%u_array%u[%u];\n",
                          cgen_ctype(
                              c, cgen_type(c, e->type_index)->first_param_type),
                          c->module,
                          i,
                          e->arg_count ? e->arg_count : 1);
            }
        }
        for (u32 i = 0; i < array_count(h->values); ++i) {
            if (!cgen_imported_value(c, &h->values[i]) &&
                !cgen_void(c, h->values[i].type_index)) {
                const HirValue* v = &h->values[i];
                const HirExpr*  e = v->value_expr_index < array_count(h->exprs)
                                        ? &h->exprs[v->value_expr_index]
                                        : NULL;
                if (e && e->kind == HIR_EXPR_Array &&
                    cgen_kind(c, v->type_index) == STK_Slice) {
                    sb_format(&c->out,
                              "static %s ncg_m%u_g%u={ncg_m%u_array%u,%u};\n",
                              cgen_ctype(c, v->type_index),
                              c->module,
                              i,
                              c->module,
                              v->value_expr_index,
                              e->arg_count);
                } else {
                    sb_format(&c->out,
                              "static %s ncg_m%u_g%u;\n",
                              cgen_ctype(c, v->type_index),
                              c->module,
                              i);
                }
            }
        }
    }
    for (c->module = 0; c->module < array_count(program->modules);
         ++c->module) {
        const Hir* h = cgen_hir(c);
        for (u32 i = 0; i < array_count(h->functions); ++i) {
            const HirFunction* fn = &h->functions[i];
            if (fn->kind == HIR_FUNCTION_Ffi) {
                continue;
            }
            cgen_reset_body(c);
            c->return_type = cgen_type(c, fn->type_index)->return_type;
            for (u32 j = 0; j < cgen_type(c, fn->type_index)->param_count;
                 ++j) {
                const HirParam* p = &h->params[fn->first_param + j];
                while (array_count(c->locals) <= p->local_index) {
                    array_push(c->locals, false);
                }
                c->locals[p->local_index] = true;
                while (array_count(c->local_types) <= p->local_index) {
                    array_push(c->local_types, U32_MAX);
                }
                c->local_types[p->local_index] =
                    cgen_field_type(c, fn->type_index, j);
                if (cgen_kind(c, p->type_index) == STK_Box) {
                    array_push(c->cleanups,
                               ((CCleanup){false, p->local_index}));
                }
            }
            if (fn->varargs_local_index != U32_MAX) {
                c->va_local = fn->varargs_local_index;
                cgen_local(c,
                           c->va_local,
                           cgen_sema(c)->locals[c->va_local].type_index);
                OUT("ncg_l%u=nrt_va_create(); "
                    "va_start(ncg_l%u->args,ncg_l%u);\n",
                    c->va_local,
                    c->va_local,
                    h->params[fn->first_param +
                              cgen_type(c, fn->type_index)->param_count - 1]
                        .local_index);
            }
            cgen_block(c, fn->body_block_index);
            cgen_return(c, cgen_temp(c, c->return_type, NULL));
            cgen_signature(c, &c->out, i, false);
            sb_append_cstr(&c->out, " {\n");
            sb_append_string(&c->out, sb_to_string(&c->declarations));
            sb_append_string(&c->out, sb_to_string(&c->body));
            sb_append_cstr(&c->out, "}\n");
        }
        cgen_reset_body(c);
        c->return_type = U32_MAX;
        c->global_init = true;
        for (u32 i = 0; i < array_count(h->values); ++i) {
            const HirValue* v = &h->values[i];
            if (cgen_imported_value(c, v) || cgen_void(c, v->type_index)) {
                continue;
            }
            CValue value = cgen_coerce(
                c, cgen_expr(c, v->value_expr_index), v->type_index);
            OUT("ncg_m%u_g%u=%s;\n", c->module, i, value.text);
        }
        sb_format(&c->out, "static void ncg_init%u(void) {\n", c->module);
        sb_append_string(&c->out, sb_to_string(&c->declarations));
        sb_append_string(&c->out, sb_to_string(&c->body));
        sb_append_cstr(&c->out, "}\n");
    }
    c->module          = program->root_module_index;
    const Hir* root    = cgen_hir(c);
    u32        main_fn = U32_MAX;
    for (u32 i = 0; i < array_count(root->bindings); ++i) {
        if (root->bindings[i].kind == HIR_BINDING_Function &&
            strcmp(cgen_symbol(c, root->bindings[i].symbol_handle), "main") ==
                0) {
            main_fn = root->bindings[i].target_index;
        }
    }
    if (main_fn == U32_MAX) {
        cgen_error(c, "missing main", 0);
    } else {
        const HirFunction* fn = &root->functions[main_fn];
        const SemaType*    ft = cgen_type(c, fn->type_index);
        sb_append_cstr(&c->out,
                       "int main(int argc, char** argv) {\nnrt_core_init();\n");
        Array(bool) done = NULL;
        for (u32 i = 0; i < array_count(program->modules); ++i) {
            array_push(done, false);
        }
        cgen_init_module(c, program->root_module_index, &done);
        array_free(done);
        cstr args = "";
        if (ft->param_count) {
            u32 t = cgen_field_type(c, fn->type_index, 0);
            sb_format(
                &c->out,
                "NerdString* args=calloc((size_t)argc,sizeof(NerdString)); "
                "for(int i=0;i<argc;++i) "
                "args[i]=(NerdString){(uint8_t*)argv[i],strlen(argv[i])};\n");
            args = CF("(%s){args,(uintptr_t)argc}", cgen_ctype(c, t));
        }
        if (cgen_void(c, ft->return_type)) {
            sb_format(&c->out,
                      "%s(%s); int result=0;\n",
                      cgen_function(c, c->module, main_fn),
                      args);
        } else {
            sb_format(&c->out,
                      "int result=(int)%s(%s);\n",
                      cgen_function(c, c->module, main_fn),
                      args);
        }
        if (ft->param_count) {
            sb_append_cstr(&c->out, "free(args);\n");
        }
        sb_append_cstr(&c->out, "nrt_core_done(); return result;\n}\n");
        if (program->windowed) {
            sb_append_cstr(&c->out,
                           "#if defined(_WIN32)\nint WINAPI WinMain(HINSTANCE "
                           "instance,HINSTANCE previous,LPSTR command,int "
                           "show) { return main(0,NULL); }\n#endif\n");
        }
    }
    bool ok = !c->failed;
    if (ok) {
        FILE* f = fopen(artifacts->binary_path, "wb");
        if (!f) {
            ok = error_runtime("Failed to open C output: %s",
                               artifacts->binary_path);
        } else {
            string text   = sb_to_string(&c->out);
            bool   wrote  = fwrite(text.data, 1, text.count, f) == text.count;
            bool   closed = fclose(f) == 0;
            if (!wrote || !closed) {
                ok = error_runtime("Failed to write C output: %s",
                                   artifacts->binary_path);
            }
        }
    }
    array_free(c->types);
    array_free(c->offsets);
    array_free(c->type_map);
    array_free(c->overrides);
    array_free(c->locals);
    array_free(c->local_types);
    array_free(c->cleanups);
    array_free(c->targets);
    arena_done(&c->out_arena);
    arena_done(&c->body_arena);
    arena_done(&c->decl_arena);
    arena_done(&c->arena);
    return ok;
}
