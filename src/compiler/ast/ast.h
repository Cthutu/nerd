//------------------------------------------------------------------------------
// AST parsing module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: compiler/error

#pragma once

#include <compiler/lexer/lexer.h>

// clang-format off
//------------------------------------------------------------------------------
// Table of AST Nodes:
//
// | Name               | a                 | b                               |
// |--------------------|-------------------|---------------------------------|
// | AK_IntegerLiteral  | Integer index     | 0                               |
// | AK_FloatLiteral    | Float index       | 0                               |
// | AK_StringLiteral   | String index      | 0                               |
// | AK_BoolLiteral     | 0 no, 1 yes       | 0                               |
// | AK_NilLiteral      | 0                 | 0                               |
// | AK_StringConcat    | Ast index of lhs  | Ast index of rhs                |
// | AK_InterpPartExpr  | Ast index of expr | 0                               |
// | AK_InterpolatedString | First part index | End-exclusive part index      |
// | AK_SymbolRef       | Symbol handle     | 0                               |
// | AK_EnumVariant     | Symbol handle     | 0                               |
// | AK_LogicalNot      | Ast index of rhs  | 0                               |
// | AK_IntegerNegate   | Ast index of rhs  | 0                               |
// | AK_AddressOf       | Ast index of value | 0                              |
// | AK_Deref           | Ast index of value | 0                              |
// | AK_IntegerPlus     | Ast index of left | Ast index of right              |
// | AK_IntegerMinus    | Ast index of left | Ast index of right              |
// | AK_IntegerMultiply | Ast index of left | Ast index of right              |
// | AK_IntegerDivide   | Ast index of left | Ast index of right              |
// | AK_IntegerModulo   | Ast index of left | Ast index of right              |
// | AK_BitwiseAnd      | Ast index of left | Ast index of right              |
// | AK_BitwiseXor      | Ast index of left | Ast index of right              |
// | AK_BitwiseOr       | Ast index of left | Ast index of right              |
// | AK_ShiftLeft       | Ast index of left | Ast index of right              |
// | AK_ShiftRight      | Ast index of left | Ast index of right              |
// | AK_Equal           | Ast index of left | Ast index of right              |
// | AK_NotEqual        | Ast index of left | Ast index of right              |
// | AK_Less            | Ast index of left | Ast index of right              |
// | AK_LessEqual       | Ast index of left | Ast index of right              |
// | AK_Greater         | Ast index of left | Ast index of right              |
// | AK_GreaterEqual    | Ast index of left | Ast index of right              |
// | AK_LogicalAnd      | Ast index of left | Ast index of right              |
// | AK_LogicalOr       | Ast index of left | Ast index of right              |
// | AK_Call            | Ast index callee  | Ast call-info index             |
// | AK_Cast            | Ast index value   | Ast cast-info index             |
// | AK_Tuple           | First item index  | Item count                      |
// | AK_TupleField      | Ast index value   | Zero-based field index          |
// | AK_Array           | First item index  | Item count                      |
// | AK_Index           | Ast index value   | Ast index of index expression   |
// | AK_Slice           | Ast slice-info index | 0                            |
// | AK_Field           | Ast index value   | Symbol handle                   |
// | AK_Plex            | Ast plex-literal index | 0                         |
// | AK_PlexUpdate      | Ast plex-literal index | 0                         |
// | AK_RangeExclusive  | Ast index start   | Ast index of end                |
// | AK_RangeInclusive  | Ast index start   | Ast index of end                |
// | AK_On              | Ast index scrutinee | Ast on-info index             |
// | AK_DestructureBind | Ast pattern index | Ast index of value              |
// | AK_DestructureVariable | Ast pattern index | Ast index of value          |
// | AK_DestructureAssign | Ast pattern index | Ast index of value            |
// | AK_TypeFn          | Ast fn-signature index | 0                           |
// | AK_TypeApply       | Ast type-apply info index | 0                         |
// | AK_TypeTuple       | First item index  | Item count                      |
// | AK_TypeArray       | Ast index length  | Ast index element type          |
// | AK_TypeSlice       | Ast index element type | 0                           |
// | AK_TypeDynamicArray | Ast index min capacity or U32_MAX | Ast index element type |
// | AK_TypePointer     | Ast index pointee type | 0                           |
// | AK_TypePlex        | Ast plex-type index | 0                            |
// | AK_TypeEnum        | Ast enum-type index | 0                            |
// | AK_Expression      | Ast index of root | 0                               |
// | AK_Statement       | Ast index of expr | 0                               |
// | AK_Return          | Ast index of expr | 0                               |
// | AK_Defer           | Ast index of deferred statement | 0                |
// | AK_Assert          | Ast index of condition | Ast string node or U32_MAX    |
// | AK_ReturnExpr      | Ast index of expr or U32_MAX | 0                    |
// | AK_BreakExpr       | Ast index of expr or U32_MAX | Symbol or U32_MAX   |
// | AK_ContinueExpr    | 0                 | Symbol or U32_MAX              |
// | AK_ExprBlock       | Ast index of block | Symbol or U32_MAX              |
// | AK_Block           | First stmt index  | End-exclusive stmt index        |
// | AK_For             | Ast for-info index | Body block index              |
// | AK_Break           | Ast index of expr or U32_MAX | Symbol or U32_MAX   |
// | AK_Continue        | 0                 | Symbol or U32_MAX              |
// | AK_Bind            | Symbol            | Ast index of type or expression |
// | AK_Variable        | Symbol            | Ast index of type/value/zero    |
// | AK_Assign          | Ast index of target | Ast index of value            |
// | AK_AnnotatedValue  | Ast index of type | Ast index of value              |
// | AK_ZeroInit        | Ast index of type | 0                               |
// | AK_Undefined       | Ast index of type | 0                               |
// | AK_FnDef           | Body start index  | Fn syntax kind                  |
// | AK_FnStart         | Ast fn-signature index | AK_FnEnd index             |
// | AK_FnEnd           | AK_FnDef index    | AK_FnStart index                |
// | AK_FfiDef          | Ast ffi-info index | 0                              |
// | AK_ModRef          | Ast module path index | 0                            |
// | AK_Use             | Ast index of module expression | 0                  |
// | AK_TopOn           | Ast top-on info index | 0                           |
//
// clang-format on

typedef enum {
    AK_IntegerLiteral,
    AK_FloatLiteral,
    AK_StringLiteral,
    AK_BoolLiteral,
    AK_NilLiteral,
    AK_StringConcat,
    AK_InterpPartExpr,
    AK_InterpolatedString,
    AK_SymbolRef,
    AK_EnumVariant,
    AK_LogicalNot,
    AK_IntegerNegate,
    AK_AddressOf,
    AK_Deref,
    AK_IntegerPlus,
    AK_IntegerMinus,
    AK_IntegerMultiply,
    AK_IntegerDivide,
    AK_IntegerModulo,
    AK_BitwiseAnd,
    AK_BitwiseXor,
    AK_BitwiseOr,
    AK_ShiftLeft,
    AK_ShiftRight,
    AK_Equal,
    AK_NotEqual,
    AK_Less,
    AK_LessEqual,
    AK_Greater,
    AK_GreaterEqual,
    AK_LogicalAnd,
    AK_LogicalOr,
    AK_Call,
    AK_Cast,
    AK_Tuple,
    AK_TupleField,
    AK_Array,
    AK_Index,
    AK_Slice,
    AK_Field,
    AK_Plex,
    AK_PlexUpdate,
    AK_RangeExclusive,
    AK_RangeInclusive,
    AK_On,
    AK_DestructureBind,
    AK_DestructureVariable,
    AK_DestructureAssign,
    AK_TypeFn,
    AK_TypeApply,
    AK_TypeTuple,
    AK_TypeArray,
    AK_TypeSlice,
    AK_TypeDynamicArray,
    AK_TypePointer,
    AK_TypePlex,
    AK_TypeEnum,
    AK_Expression,
    AK_Statement,
    AK_Return,
    AK_Defer,
    AK_Assert,
    AK_ReturnExpr,
    AK_BreakExpr,
    AK_ContinueExpr,
    AK_ExprBlock,
    AK_Block,
    AK_For,
    AK_Break,
    AK_Continue,
    AK_Bind,
    AK_Variable,
    AK_Assign,
    AK_AnnotatedValue,
    AK_ZeroInit,
    AK_Undefined,
    AK_FnDef,
    AK_FnStart,
    AK_FnEnd,
    AK_FfiDef,
    AK_ModRef,
    AK_Use,
    AK_TopOn,
} AstKind;

typedef enum : u32 {
    AFK_Expr,
    AFK_Block,
} AstFnKind;

typedef enum : u8 {
    ANF_None       = 0,
    ANF_ConstKnown = 1 << 0,
    ANF_ConstBusy  = 1 << 1,
    ANF_Public     = 1 << 2,
} AstNodeFlag;

typedef struct {
    AstKind kind : 8;
    u8      flags;
    u16     _pad2;
    u32     token_index;
    u32     a, b; // Meaning depends on the AstKind
} AstNode;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 type_node_index;
    u32 default_node_index;
} AstParam;

typedef struct {
    u32 first_symbol;
    u32 symbol_count;
} AstGenericParams;

typedef struct {
    u32  first_param;
    u32  param_count;
    u32  return_type_node_index;
    u32  generic_params_index;
    bool is_varargs;
} AstFnSignature;

typedef struct {
    u32 first_arg;
    u32 arg_count;
} AstCallInfo;

typedef struct {
    u32 type_node_index;
    u32 extra_node_index;
} AstCastInfo;

typedef struct {
    u32 target_node_index;
    u32 first_arg;
    u32 arg_count;
} AstTypeApplyInfo;

typedef struct {
    u32 library_node_index;
    u32 symbol_handle;
    u32 signature_index;
} AstFfiInfo;

typedef struct {
    u32 first_symbol;
    u32 symbol_count;
} AstModulePath;

typedef struct {
    u32 target_node_index;
    u32 start_node_index;
    u32 end_node_index;
} AstSliceInfo;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 type_node_index;
} AstPlexField;

typedef struct {
    u32 first_field;
    u32 field_count;
    u32 generic_params_index;
    u32 flags;
} AstPlexTypeInfo;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 type_node_index;
    u32 value_node_index;
} AstEnumVariant;

typedef struct {
    u32 first_variant;
    u32 variant_count;
    u32 generic_params_index;
} AstEnumTypeInfo;

typedef enum : u32 {
    APTF_None   = 0,
    APTF_C      = 1 << 0,
    APTF_Packed = 1 << 1,
    APTF_Union  = 1 << 2,
} AstPlexTypeFlag;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 value_node_index;
} AstPlexLiteralField;

typedef enum : u32 {
    APLF_None        = 0,
    APLF_ZeroMissing = 1 << 0,
} AstPlexLiteralFlag;

typedef struct {
    u32 target_node_index;
    u32 first_field;
    u32 field_count;
    u32 flags;
} AstPlexLiteralInfo;

typedef enum : u32 {
    APK_Value,
    APK_Ignore,
    APK_Bind,
    APK_Equal,
    APK_NotEqual,
    APK_Less,
    APK_LessEqual,
    APK_Greater,
    APK_GreaterEqual,
    APK_RangeExclusive,
    APK_RangeInclusive,
    APK_Tuple,
    APK_Plex,
    APK_EnumVariant,
} AstPatternKind;

typedef struct {
    AstPatternKind kind;
    u32            token_index;
    u32            a;
    u32            b;
} AstPattern;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 pattern_index;
} AstPlexPatternField;

typedef struct {
    u32 token_index;
    u32 qualifier_node_index;
    u32 symbol_handle;
    u32 first_pattern;
    u32 pattern_count;
} AstEnumPattern;

typedef struct {
    u32 pattern_index;
    u32 expr_node_index;
    u32 pattern_count;
    u32 guard_node_index;
    u32 flags;
    u32 binder_symbol_handle;
    u32 binder_token_index;
} AstOnBranch;

typedef enum : u32 {
    AOK_Bool,
    AOK_Value,
    AOK_Condition,
} AstOnKind;

typedef enum : u32 {
    AOBF_None = 0,
    AOBF_Else = 1 << 0,
} AstOnBranchFlag;

typedef struct {
    AstOnKind kind;
    u32       first_branch;
    u32       branch_count;
} AstOnInfo;

typedef struct {
    u32  symbol_handle;
    u32  body_node_index;
    bool is_negated;
} AstTopOnInfo;

typedef enum : u32 {
    AFM_Condition,
    AFM_CStyle,
    AFM_In,
} AstForMode;

typedef struct {
    u32  mode;
    u32  first_init;
    u32  init_count;
    u32  condition_node_index;
    u32  first_update;
    u32  update_count;
    u32  iterable_node_index;
    u32  index_symbol;
    u32  index_token_index;
    u32  item_symbol;
    u32  item_token_index;
    u32  label_symbol;
    u32  else_block_index;
    bool item_is_pointer;
} AstForInfo;

typedef struct {
    Array(AstNode) nodes;
    Array(AstGenericParams) generic_params;
    Array(u32) generic_param_symbols;
    Array(AstParam) params;
    Array(AstFnSignature) fn_signatures;
    Array(AstFfiInfo) ffi_infos;
    Array(AstModulePath) module_paths;
    Array(u32) module_path_symbols;
    Array(u32) call_args;
    Array(u32) tuple_items;
    Array(AstCallInfo) calls;
    Array(AstCastInfo) casts;
    Array(AstTypeApplyInfo) type_applications;
    Array(AstSliceInfo) slices;
    Array(AstPlexField) plex_fields;
    Array(AstPlexTypeInfo) plex_types;
    Array(AstEnumVariant) enum_variants;
    Array(AstEnumTypeInfo) enum_types;
    Array(AstPlexLiteralField) plex_literal_fields;
    Array(AstPlexLiteralInfo) plex_literals;
    Array(AstPattern) patterns;
    Array(u32) pattern_items;
    Array(AstPlexPatternField) pattern_fields;
    Array(AstEnumPattern) enum_patterns;
    Array(AstOnBranch) on_branches;
    Array(AstOnInfo) ons;
    Array(AstTopOnInfo) top_ons;
    Array(u32) for_items;
    Array(AstForInfo) fors;
} Ast;

Ast  ast_parse(Lexer* lexer);
void ast_done(Ast* ast);
void ast_dump(const Ast* ast, const Lexer* lexer);

//------------------------------------------------------------------------------
// Extraction API

u64    ast_get_integer(const Lexer* lexer, const AstNode* node);
f64    ast_get_float(const Lexer* lexer, const AstNode* node);
string ast_get_string(const Lexer* lexer, const AstNode* node);
u32    ast_get_symbol(const AstNode* node);
bool   ast_node_is_block_statement(const AstNode* node);
u32    ast_block_statement_end_exclusive(const Ast* ast, u32 node_index);

//------------------------------------------------------------------------------
// AST node flag helpers

static inline bool ast_has_flag(const AstNode* node, AstNodeFlag flag)
{
    return (node->flags & flag) != 0;
}

static inline void ast_set_flag(AstNode* node, AstNodeFlag flag)
{
    node->flags |= flag;
}

static inline void ast_clear_flag(AstNode* node, AstNodeFlag flag)
{
    node->flags &= (u8)~flag;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
