//------------------------------------------------------------------------------
// Concrete syntax tree module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

#include <compiler/lexer/lexer.h>

//------------------------------------------------------------------------------
// Table of CST Nodes:
//
// | Name               | a                     | b                     |
// |--------------------|-----------------------|-----------------------|
// | CK_IntegerLiteral  | Integer index         | 0                     |
// | CK_FloatLiteral    | Float index           | 0                     |
// | CK_StringLiteral   | Lexer string index    | 0                     |
// | CK_BoolLiteral     | 0 no, 1 yes           | 0                     |
// | CK_NilLiteral      | 0                     | 0                     |
// | CK_StringConcat    | Left node index       | Right node index      |
// | CK_InterpPartExpr  | Expr node index       | 0                     |
// | CK_InterpolatedString | First part index    | End-exclusive index   |
// | CK_SymbolRef       | Symbol handle         | 0                     |
// | CK_Group           | Inner node index      | 0                     |
// | CK_LogicalNot      | Operand node index    | 0                     |
// | CK_IntegerNegate   | Operand node index    | 0                     |
// | CK_AddressOf       | Operand node index    | 0                     |
// | CK_Deref           | Operand node index    | 0                     |
// | CK_IntegerPlus     | Left node index       | Right node index      |
// | CK_IntegerMinus    | Left node index       | Right node index      |
// | CK_IntegerMultiply | Left node index       | Right node index      |
// | CK_IntegerDivide   | Left node index       | Right node index      |
// | CK_IntegerModulo   | Left node index       | Right node index      |
// | CK_BitwiseAnd      | Left node index       | Right node index      |
// | CK_BitwiseXor      | Left node index       | Right node index      |
// | CK_BitwiseOr       | Left node index       | Right node index      |
// | CK_ShiftLeft       | Left node index       | Right node index      |
// | CK_ShiftRight      | Left node index       | Right node index      |
// | CK_Equal           | Left node index       | Right node index      |
// | CK_NotEqual        | Left node index       | Right node index      |
// | CK_Less            | Left node index       | Right node index      |
// | CK_LessEqual       | Left node index       | Right node index      |
// | CK_Greater         | Left node index       | Right node index      |
// | CK_GreaterEqual    | Left node index       | Right node index      |
// | CK_LogicalAnd      | Left node index       | Right node index      |
// | CK_LogicalOr       | Left node index       | Right node index      |
// | CK_Call            | Callee node index     | Call-info index       |
// | CK_Cast            | Value node index      | Cast-info index       |
// | CK_Tuple           | First item index      | Item count            |
// | CK_TupleField      | Value node index      | Zero-based field      |
// | CK_Array           | First item index      | Item count            |
// | CK_Index           | Value node index      | Index expr node       |
// | CK_Slice           | Slice-info index      | 0                     |
// | CK_Field           | Value node index      | Symbol handle         |
// | CK_Plex            | Plex-literal index    | 0                     |
// | CK_PlexUpdate      | Plex-literal index    | 0                     |
// | CK_RangeExclusive  | Start node index      | End node index        |
// | CK_RangeInclusive  | Start node index      | End node index        |
// | CK_On              | Scrutinee node index  | On-info index         |
// | CK_DestructureBind | Pattern index         | Value node index      |
// | CK_DestructureVariable | Pattern index      | Value node index      |
// | CK_DestructureAssign | Pattern index        | Value node index      |
// | CK_TypeFn          | Fn-signature index    | 0                     |
// | CK_TypeApply       | Type-apply info index | 0                     |
// | CK_TypeTuple       | First item index      | Item count            |
// | CK_TypeArray       | Length node index     | Element type node     |
// | CK_TypeSlice       | Element type node     | 0                     |
// | CK_TypeDynamicArray | Min capacity node or U32_MAX | Element type node |
// | CK_TypePointer     | Pointee type node     | 0                     |
// | CK_TypePlex        | Plex-type index       | 0                     |
// | CK_FnExpr          | Fn-signature index    | Body node index       |
// | CK_FnBlock         | Fn-signature index    | Block node index      |
// | CK_FfiDef          | FFI-info index        | 0                     |
// | CK_FfiBlock        | FFI-block info index  | 0                     |
// | CK_ModRef          | Module path index     | 0                     |
// | CK_Use             | Module node index     | 0                     |
// | CK_Impl            | Impl-info index       | 0                     |
// | CK_Trait           | Trait-info index      | 0                     |
// | CK_TopOn           | Top-on/assert-on info index | 0              |
// | CK_Pragma          | Pragma-info index     | 0                     |
// | CK_Test            | Name string node      | Body block node       |
// | CK_Statement       | Expr node index       | 0                     |
// | CK_Return          | Expr node index       | 0                     |
// | CK_Defer           | Deferred statement node index | 0              |
// | CK_Assert          | Condition node index  | Message expr or U32_MAX |
// | CK_ReturnExpr      | Expr node or U32_MAX  | 0                     |
// | CK_BreakExpr       | Expr node or U32_MAX  | Symbol or U32_MAX    |
// | CK_BreakOn         | Condition node index  | Break expr node      |
// | CK_ContinueExpr    | 0                     | Symbol or U32_MAX    |
// | CK_ExprBlock       | Block node index      | Symbol or U32_MAX    |
// | CK_Block           | First stmt index      | End-exclusive index   |
// | CK_For             | CST for-info index    | Body block node      |
// | CK_Break           | Expr node or U32_MAX  | Symbol or U32_MAX    |
// | CK_Continue        | 0                     | Symbol or U32_MAX    |
// | CK_AnnotatedValue  | Type node index       | Value node index      |
// | CK_ZeroInit        | Type node index       | 0                     |
// | CK_Undefined       | Type node index       | 0                     |
// | CK_Bind            | Symbol handle         | Value node index      |
// | CK_Variable        | Symbol handle         | Value node index      |
// | CK_Assign          | Target node index     | Value node index      |

typedef enum {
    CK_IntegerLiteral,
    CK_FloatLiteral,
    CK_StringLiteral,
    CK_BoolLiteral,
    CK_NilLiteral,
    CK_StringConcat,
    CK_InterpPartExpr,
    CK_InterpolatedString,
    CK_SymbolRef,
    CK_EnumVariant,
    CK_Group,
    CK_LogicalNot,
    CK_IntegerNegate,
    CK_AddressOf,
    CK_Deref,
    CK_IntegerPlus,
    CK_IntegerMinus,
    CK_IntegerMultiply,
    CK_IntegerDivide,
    CK_IntegerModulo,
    CK_BitwiseAnd,
    CK_BitwiseXor,
    CK_BitwiseOr,
    CK_ShiftLeft,
    CK_ShiftRight,
    CK_Equal,
    CK_NotEqual,
    CK_Less,
    CK_LessEqual,
    CK_Greater,
    CK_GreaterEqual,
    CK_LogicalAnd,
    CK_LogicalOr,
    CK_Call,
    CK_Cast,
    CK_Tuple,
    CK_TupleField,
    CK_Array,
    CK_Index,
    CK_Slice,
    CK_Field,
    CK_Plex,
    CK_PlexUpdate,
    CK_RangeExclusive,
    CK_RangeInclusive,
    CK_On,
    CK_DestructureBind,
    CK_DestructureVariable,
    CK_DestructureAssign,
    CK_TypeFn,
    CK_TypeApply,
    CK_TypeTuple,
    CK_TypeArray,
    CK_TypeSlice,
    CK_TypeDynamicArray,
    CK_TypePointer,
    CK_TypePlex,
    CK_TypeEnum,
    CK_FnExpr,
    CK_FnBlock,
    CK_FfiDef,
    CK_FfiBlock,
    CK_ModRef,
    CK_Use,
    CK_Impl,
    CK_Trait,
    CK_TopOn,
    CK_Pragma,
    CK_Test,
    CK_Statement,
    CK_Return,
    CK_Defer,
    CK_Assert,
    CK_ReturnExpr,
    CK_BreakExpr,
    CK_BreakOn,
    CK_ContinueExpr,
    CK_ExprBlock,
    CK_Block,
    CK_For,
    CK_Break,
    CK_Continue,
    CK_AnnotatedValue,
    CK_ZeroInit,
    CK_Undefined,
    CK_Bind,
    CK_Variable,
    CK_Assign,
} CstKind;

typedef struct {
    CstKind kind : 8;
    u8      flags;
    u16     _pad2;
    u32     token_index;
    u32     a, b;
} CstNode;

typedef enum : u8 {
    CNF_None   = 0,
    CNF_Public = 1 << 0,
} CstNodeFlag;

typedef struct {
    u32 first_symbol;
    u32 symbol_count;
} CstGenericParams;

typedef struct {
    u32 token_index;
    u32 param_symbol;
    u32 trait_type_node_index;
} CstWhereConstraint;

typedef struct {
    u32 symbol_handle;
    u32 type_node_index;
    u32 default_node_index;
} CstParam;

typedef struct {
    u32  first_param;
    u32  param_count;
    u32  return_type_node_index;
    u32  generic_params_index;
    u32  first_constraint;
    u32  constraint_count;
    bool is_varargs;
} CstFnSignature;

typedef struct {
    u32 token_index;
    u32 library_node_index;
    u32 symbol_handle;
    u32 foreign_symbol_handle;
    u32 signature_index;
    u8  flags;
} CstFfiInfo;

typedef struct {
    u32 library_node_index;
    u32 first_ffi_info;
    u32 ffi_info_count;
    u8  flags;
} CstFfiBlockInfo;

typedef struct {
    u32 first_symbol;
    u32 symbol_count;
} CstModulePath;

typedef struct {
    u32 first_arg;
    u32 arg_count;
} CstCallInfo;

typedef struct {
    u32 type_node_index;
    u32 extra_node_index;
} CstCastInfo;

typedef struct {
    u32 target_node_index;
    u32 first_arg;
    u32 arg_count;
} CstTypeApplyInfo;

typedef struct {
    u32 target_node_index;
    u32 start_node_index;
    u32 end_node_index;
} CstSliceInfo;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 type_node_index;
} CstPlexField;

typedef struct {
    u32 first_field;
    u32 field_count;
    u32 generic_params_index;
    u32 flags;
} CstPlexTypeInfo;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 type_node_index;
    u32 value_node_index;
} CstEnumVariant;

typedef struct {
    u32 first_variant;
    u32 variant_count;
    u32 generic_params_index;
} CstEnumTypeInfo;

typedef struct {
    u32 body_node_index;
    u32 self_alias_symbol;
    u32 generic_params_index;
} CstTraitInfo;

typedef enum : u32 {
    CPTF_None   = 0,
    CPTF_C      = 1 << 0,
    CPTF_Packed = 1 << 1,
    CPTF_Union  = 1 << 2,
} CstPlexTypeFlag;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 value_node_index;
} CstPlexLiteralField;

typedef enum : u32 {
    CPLF_None           = 0,
    CPLF_DefaultMissing = 1 << 0,
} CstPlexLiteralFlag;

typedef struct {
    u32 target_node_index;
    u32 first_field;
    u32 field_count;
    u32 flags;
} CstPlexLiteralInfo;

typedef enum : u32 {
    CPK_Value,
    CPK_Ignore,
    CPK_Bind,
    CPK_Equal,
    CPK_NotEqual,
    CPK_Less,
    CPK_LessEqual,
    CPK_Greater,
    CPK_GreaterEqual,
    CPK_RangeExclusive,
    CPK_RangeInclusive,
    CPK_Tuple,
    CPK_Plex,
    CPK_EnumVariant,
} CstPatternKind;

typedef struct {
    CstPatternKind kind;
    u32            token_index;
    u32            a;
    u32            b;
    u32            c;
} CstPattern;

typedef struct {
    u32 token_index;
    u32 symbol_handle;
    u32 pattern_index;
} CstPlexPatternField;

typedef struct {
    u32 token_index;
    u32 qualifier_node_index;
    u32 symbol_handle;
    u32 first_pattern;
    u32 pattern_count;
} CstEnumPattern;

typedef struct {
    u32 pattern_index;
    u32 expr_node_index;
    u32 pattern_count;
    u32 guard_node_index;
    u32 flags;
    u32 binder_symbol_handle;
    u32 binder_token_index;
} CstOnBranch;

typedef enum : u32 {
    COK_Bool,
    COK_Value,
    COK_Condition,
} CstOnKind;

typedef enum : u32 {
    COBF_None = 0,
    COBF_Else = 1 << 0,
} CstOnBranchFlag;

typedef struct {
    CstOnKind kind;
    u32       first_branch;
    u32       branch_count;
} CstOnInfo;

typedef struct {
    u32  string_index;
    u32  body_node_index;
    bool is_negated;
    bool is_assert;
} CstTopOnInfo;

typedef enum : u32 {
    CPPK_Integer,
    CPPK_Float,
    CPPK_String,
    CPPK_Bool,
} CstPragmaParamKind;

typedef struct {
    CstPragmaParamKind kind;
    u32                token_index;
    u32                value_index;
    bool               bool_value;
} CstPragmaParam;

typedef struct {
    u32 symbol_handle;
    u32 first_param;
    u32 param_count;
} CstPragmaInfo;

typedef struct {
    u32 trait_type_node_index;
    u32 target_type_node_index;
    u32 body_node_index;
    u32 generic_params_index;
    u32 first_constraint;
    u32 constraint_count;
} CstImplInfo;

typedef enum : u32 {
    CFM_Condition,
    CFM_CStyle,
    CFM_In,
} CstForMode;

typedef struct {
    u32 mode;
    u32 first_init;
    u32 init_count;
    u32 condition_node_index;
    u32 first_update;
    u32 update_count;
    u32 iterable_node_index;
    u32 index_symbol;
    u32 index_token_index;
    u32 item_symbol;
    u32 item_token_index;
    u32 label_symbol;
    u32 else_block_index;
} CstForInfo;

typedef struct {
    Array(CstNode) nodes;
    Array(CstGenericParams) generic_params;
    Array(u32) generic_param_symbols;
    Array(CstWhereConstraint) where_constraints;
    Array(u64) integers;
    Array(f64) floats;
    Array(u32) bindings;
    Array(CstParam) params;
    Array(CstFnSignature) fn_signatures;
    Array(CstFfiInfo) ffi_infos;
    Array(CstFfiBlockInfo) ffi_block_infos;
    Array(CstModulePath) module_paths;
    Array(u32) module_path_symbols;
    Array(u32) call_args;
    Array(u32) tuple_items;
    Array(CstCallInfo) calls;
    Array(CstCastInfo) casts;
    Array(CstTypeApplyInfo) type_applications;
    Array(CstSliceInfo) slices;
    Array(CstPlexField) plex_fields;
    Array(CstPlexTypeInfo) plex_types;
    Array(CstEnumVariant) enum_variants;
    Array(CstEnumTypeInfo) enum_types;
    Array(CstTraitInfo) trait_infos;
    Array(CstPlexLiteralField) plex_literal_fields;
    Array(CstPlexLiteralInfo) plex_literals;
    Array(CstPattern) patterns;
    Array(u32) pattern_items;
    Array(CstPlexPatternField) pattern_fields;
    Array(CstEnumPattern) enum_patterns;
    Array(CstOnBranch) on_branches;
    Array(CstOnInfo) ons;
    Array(CstTopOnInfo) top_ons;
    Array(CstPragmaParam) pragma_params;
    Array(CstPragmaInfo) pragmas;
    Array(CstImplInfo) impls;
    Array(u32) for_items;
    Array(CstForInfo) fors;
} Cst;

bool cst_parse(const Lexer* lexer, Cst* out_cst);
void cst_done(Cst* cst);

u64  cst_get_integer(const Cst* cst, const CstNode* node);
f64  cst_get_float(const Cst* cst, const CstNode* node);
u32  cst_get_symbol(const CstNode* node);
bool cst_node_is_binding_like(const CstNode* node);
bool cst_node_is_block_statement(const CstNode* node);
u32  cst_block_statement_end_exclusive(const Cst* cst, u32 node_index);

//------------------------------------------------------------------------------
