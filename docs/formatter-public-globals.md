# Preserve public mutable declarations

The structured formatter must retain declaration visibility independently of
alignment groups. Previously, standalone mutable declarations such as
`pub count: i32 = 0` became `count: i32 = 0`. Adjacent aligned declarations already
preserved `pub`, so adding or removing a blank line could change whether
formatting removed an export. Inferred mutable declarations inside platform
guards also lost their visibility.

The standalone root emitter and `CK_Variable` statement emitter now write the
`CNF_Public` prefix. The statement emitter includes its four columns when
deciding where a long initialiser call wraps. Constant bindings and private
variables retain their existing formatting rules; the CST visibility flag and
semantic export rules are unchanged.

`tests/format/212-public-mutable-globals.f` covers typed initialised, inferred,
zero-initialised and undefined public mutable globals, typed/inferred public
constants, private declarations, mixed alignment groups and platform guards.
The normal format runner checks both expected output and a second, idempotent
format pass. A separate manual semantic smoke check formats a module containing
one public typed mutable global, then checks an importing source that reads and
writes that export.

Native Windows verification used a fresh Clang debug compiler. The unmodified
compiler fails the new fixture; the repaired compiler passes all 212 format
fixtures (including idempotence), three format-related command fixtures and the
formatted-module import read/write semantic smoke check. The complete integrated
compiler/platform gate remains the integration branch's responsibility.

This is a formatter repair. The separately tracked LLVM imported-global address
and imported constant C-output issues require their own compiler regressions.
