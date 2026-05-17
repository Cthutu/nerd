//------------------------------------------------------------------------------
// Semantic analysis module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/ast/ast.h>
#include <compiler/compiler.h>

//------------------------------------------------------------------------------

typedef enum : u8 {
    SK_Constant,
    SK_Variable,
    SK_TypeAlias,
    SK_Function,
    SK_FfiFunction,
    SK_Module,
    SK_BuiltinFunction,
    SK_GenericTypeAlias,
    SK_GenericFunction,
    SK_Trait,
} SemaDeclKind;

typedef enum : u8 {
    STK_Void,
    STK_UntypedInteger,
    STK_UntypedFloat,
    STK_Nil,
    STK_String,
    STK_Bool,
    STK_I8,
    STK_I16,
    STK_I32,
    STK_I64,
    STK_U8,
    STK_U16,
    STK_U32,
    STK_U64,
    STK_F32,
    STK_F64,
    STK_Isize,
    STK_Usize,
    STK_Arena,
    STK_Function,
    STK_Module,
    STK_Tuple,
    STK_Array,
    STK_Slice,
    STK_DynamicArray,
    STK_Pointer,
    STK_Plex,
    STK_Union,
    STK_Enum,
} SemaTypeKind;

//------------------------------------------------------------------------------
// One semantic type row stored outside the AST.

typedef struct {
    SemaTypeKind kind;
    u16          param_count;
    u16          flags;
    u32          first_param_type;
    u32          return_type;
} SemaType;

typedef enum : u16 {
    STF_None            = 0,
    STF_PlexC           = 1 << 0,
    STF_PlexPacked      = 1 << 1,
    STF_FunctionVarargs = 1 << 2,
} SemaTypeFlag;

//------------------------------------------------------------------------------
// A top-level declaration collected from an AK_Bind node.

typedef struct {
    SemaDeclKind kind;
    u32          symbol_handle;
    u32          bind_node_index;
    u32          type_node_index;
    u32          value_node_index;
    u32          type_index;
    u32          import_module_index;
    u32          import_decl_index;
} SemaDecl;

typedef enum : u8 {
    SLK_Param,
    SLK_Variable,
    SLK_Binder,
    SLK_Constant,
    SLK_Function,
    SLK_TypeAlias,
} SemaLocalKind;

typedef struct {
    SemaLocalKind kind;
    u8            _pad0;
    u16           _pad1;
    u32           symbol_handle;
    u32           owner_decl_index;
    u32           scope_index;
    u32           decl_node_index;
    u32           decl_token_index;
    u32           type_node_index;
    u32           value_node_index;
    u32           type_index;
    u32           lowered_symbol_handle;
} SemaLocal;

typedef struct {
    u32 owner_decl_index;
    u32 parent_scope_index;
    u32 first_local;
    u32 local_count;
} SemaScope;

typedef struct {
    u32  symbol_handle;
    u32  decl_index;
    u32  impl_node_index;
    u32  target_type_node_index;
    u32  generic_params_index;
    bool is_public;
    bool is_associated;
    bool first_param_is_receiver;
    bool returns_self;
    bool is_trait_impl;
} SemaMethod;

//------------------------------------------------------------------------------
// A dependency edge between two top-level declarations.

typedef struct {
    u32 from_decl_index;
    u32 to_decl_index;
} SemaDeclDep;

typedef struct {
    u32 symbol_handle;
    u32 template_decl_index;
    u32 fn_node_index;
    u32 root_scope_index;
    u32 type_index;
    u32 first_arg_type;
    u32 arg_count;
    Array(u32) node_decl_indices;
    Array(u32) node_local_indices;
    Array(u32) node_scope_indices;
    Array(u32) node_lowered_symbol_handles;
    Array(u32) node_type_indices;
    Array(u32) node_method_call_decl_indices;
    Array(bool) node_method_call_receiver_refs;
    Array(bool) node_method_call_receiver_derefs;
    Array(bool) node_method_call_explicit_traits;
} SemaGenericFnInstantiation;

//------------------------------------------------------------------------------
// Compact semantic side tables keyed by declaration and AST node index.

typedef struct {
    const ProgramInfo* program;
    Array(SemaType) types;
    Array(u32) type_param_types;
    Array(u32) type_param_symbols;
    Array(i64) type_param_values;
    Array(SemaDecl) decls;
    Array(SemaGenericFnInstantiation) generic_fn_instantiations;
    Array(SemaMethod) methods;
    Array(SemaLocal) locals;
    Array(SemaScope) scopes;
    Array(SemaDeclDep) deps;
    Array(u32) ordered_decl_indices;
    Array(u32) node_decl_indices;
    Array(u32) node_local_indices;
    Array(u32) node_scope_indices;
    Array(u32) node_lowered_symbol_handles;
    Array(u32) node_type_indices;
    Array(u32) node_method_call_decl_indices;
    Array(bool) node_method_call_receiver_refs;
    Array(bool) node_method_call_receiver_derefs;
    Array(bool) node_method_call_explicit_traits;
    Array(u32) node_implicit_array_type_indices;
    Array(u32) on_branch_local_indices;
    Array(u32) pattern_local_indices;
    Array(bool) node_is_type_expr;
    Array(bool) node_const_known;
    Array(i64) node_const_values;
} Sema;

//------------------------------------------------------------------------------

bool sema_analyse(const Lexer*           lexer,
                  Ast*                   ast,
                  const FrontEndOptions* options,
                  Sema*                  out_sema);
void sema_done(Sema* sema);

u32 sema_import_symbol_handle(Lexer*       dst_lexer,
                              const Lexer* src_lexer,
                              u32          src_symbol_handle);
u32 sema_import_type(Lexer*       dst_lexer,
                     Sema*        dst_sema,
                     const Lexer* src_lexer,
                     const Sema*  src_sema,
                     u32          src_type_index);

u32    sema_no_decl(void);
u32    sema_no_local(void);
u32    sema_no_type(void);
u32    sema_materialise_type(const Sema* sema, u32 type_index);
bool   sema_type_is_integer(const Sema* sema, u32 type_index);
bool   sema_type_is_concrete_integer(const Sema* sema, u32 type_index);
bool   sema_type_is_float(const Sema* sema, u32 type_index);
string sema_type_name(const Lexer* lexer,
                      const Sema*  sema,
                      Arena*       arena,
                      u32          type_index);

//------------------------------------------------------------------------------
