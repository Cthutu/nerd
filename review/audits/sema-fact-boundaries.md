# Sema Fact Boundary Audit

Status: active
Date: 2026-05-12

## Purpose

Milestone 1 is about splitting semantic facts from semantic success. The
compiler currently has one `Sema` product that contains declaration, binding,
scope, type, dependency, constant, method, generic-instantiation, and
lowering-support tables. That is convenient for batch compilation, but it is too
broad for editor tooling because the LSP often needs useful declaration and
binding facts while the source is incomplete or type checking has failed.

The goal is not to delete `Sema` immediately. The first step is to classify the
facts it already owns and define which facts should become explicit product
views.

## Current Product

`Sema` currently owns these table families:

- `types`, `type_param_types`, `type_param_symbols`, `type_param_values`
- `decls`
- `generic_fn_instantiations`
- `methods`
- `locals`
- `scopes`
- `deps`, `ordered_decl_indices`
- AST-node side tables:
  - `node_decl_indices`
  - `node_local_indices`
  - `node_scope_indices`
  - `node_lowered_symbol_handles`
  - `node_type_indices`
  - `node_method_call_decl_indices`
  - `node_method_call_receiver_refs`
  - `node_method_call_receiver_derefs`
  - `node_implicit_array_type_indices`
  - `node_is_type_expr`
  - `node_const_known`
  - `node_const_values`
- pattern and `on` side tables:
  - `on_branch_local_indices`
  - `pattern_local_indices`

The front-end currently reports one semantic product state:

```c
FrontEndReadiness.sema = Missing | Partial | Complete
```

The LSP derives:

```c
sema_partial
sema_complete
```

That distinction is useful, but still too coarse. A partial sema product does
not say which table families are safe.

## Fact Classification

### Declaration Facts

Tables:

- `decls`
- module export lists in `ProgramInfo`
- import metadata on `SemaDecl`
- public/private declaration state inferred from AST and module records

Consumers:

- document symbols
- completion
- hover
- definition
- rename
- module completion
- HIR generation
- LLVM lowering through HIR-owned declarations and bindings

Desired readiness:

- available after top-level declaration collection
- should not require successful type checking
- entries may have unknown or unchecked `type_index` until type facts are ready

### Binding And Reference Facts

Tables:

- `node_decl_indices`
- `node_local_indices`
- `node_scope_indices`
- local declaration rows
- symbol handles on declarations and locals
- module alias/import references

Consumers:

- completion visibility
- hover
- definition
- rename
- semantic tokens
- code actions
- HIR generation

Desired readiness:

- available after scope construction and name resolution
- should tolerate unresolved references as missing rows, not as invalid table
  access
- should distinguish source binding identity from lowered HIR entity identity

### Scope Facts

Tables:

- `scopes`
- `locals`
- `node_scope_indices`
- local first/count ranges

Consumers:

- completion
- rename
- hover
- definition
- definite assignment
- HIR local lowering

Desired readiness:

- available after body scope construction
- local rows should exist for parameters, local bindings, pattern binders, loop
  items, and nested functions even if types are unknown
- checked type facts should be optional on each row

### Type Facts

Tables:

- `types`
- `type_param_*`
- `node_type_indices`
- `node_implicit_array_type_indices`
- method call side tables
- `SemaDecl.type_index`
- `SemaLocal.type_index`

Consumers:

- diagnostics
- completion members and methods
- hover type text
- signature help
- code actions
- HIR generation
- LLVM lowering through HIR type references

Desired readiness:

- best-effort type facts may be available during LSP analysis
- checked type facts are required for HIR generation
- missing or unknown type rows should be represented explicitly by accessors
  returning false, not by callers reading arrays directly

### Dependency And Ordering Facts

Tables:

- `deps`
- `ordered_decl_indices`
- generic instantiation dependency records

Consumers:

- semantic diagnostics
- type inference and constant evaluation order
- HIR generation order

Desired readiness:

- checked semantic product only
- not required for basic LSP source navigation
- should remain separate from C/LLVM emission ordering

### Constant And Definite-Assignment Facts

Tables:

- `node_const_known`
- `node_const_values`
- local assignment state computed during checking

Consumers:

- diagnostics
- type checking
- HIR lowering of constants

Desired readiness:

- checked semantic product only
- not part of the minimum LSP declaration/binding view

### Lowering-Only Facts

Tables:

- `node_lowered_symbol_handles`
- generic instantiations
- method call receiver ref/deref flags
- implicit array type rows

Consumers:

- HIR generation
- LLVM lowering indirectly through HIR
- some LSP method/member helpers today

Desired readiness:

- should be treated as checked or best-effort type facts
- should not be the basis for source-level rename or definition
- LSP should use explicit accessors when it wants these details

## Current LSP Consumer Pattern

The LSP already has product views:

- `LspSourceView`
- `LspTokenView`
- `LspSyntaxView`
- `LspSemanticView`
- `LspModuleView`

It also has checked row helpers:

- `lsp_sema_decl`
- `lsp_sema_local`
- `lsp_sema_scope`
- `lsp_sema_type`
- node-to-decl/local/scope/type helpers
- module export helpers

Those helpers are the right direction, but their names still describe the
backing table rather than the semantic contract. Feature code can still receive
`LspSemanticView` and then read `view.sema->decls`, `locals`, `types`, or
`methods` directly.

## Proposed Product Views

Introduce these LSP/front-end-facing contracts first, backed by the existing
`Sema` tables:

```c
typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspDeclarationView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspBindingView;

typedef struct {
    const LspDocument* doc;
    string             source;
    const Lexer*       lexer;
    const Ast*         ast;
    const Sema*        sema;
} LspTypeFactView;
```

The first implementation may alias these to the same backing data. The value is
that feature handlers request the weakest product they need:

- document symbols: declaration view
- rename and definition: binding view
- completion: binding view, plus optional type fact view for receiver members
- hover: binding view, plus optional type fact view for type text
- signature help: type fact view
- code actions: syntax or type fact view depending on action
- semantic tokens: token/syntax view, plus optional declaration/binding facts

## Implementation Slices

1. Add declaration, binding, and type-fact view structs and getters.
2. Add semantic row helpers with contract names, for example
   `lsp_decl_view_decl`, `lsp_binding_view_node_decl`,
   `lsp_type_view_node_type`.
3. Keep the old `lsp_sema_*` helpers as implementation details or compatibility
   wrappers while migrating features.
4. Split `LspDocument` readiness from `sema_partial` into:
   - `decls_ready`
   - `bindings_ready`
   - `type_facts_partial`
   - `type_facts_complete`
5. Update `FrontEndReadiness` after the first compiler-side split is possible.

## Risks

- A view rename alone does not make partial sema safer. Accessors must keep
  validating indices and missing rows.
- The current sema pass may not preserve useful declaration/binding rows if it
  fails too early. That must be measured with LSP regression cases.
- Some LSP features genuinely need best-effort type facts. They should request
  that explicitly rather than silently assuming all semantic facts are checked.

## Immediate Recommendation

Start with LSP-facing declaration/binding/type-fact views backed by the current
`Sema`. This is low risk, gives feature code a clearer contract, and prepares
the later compiler split without forcing a large `sema.c` rewrite first.
