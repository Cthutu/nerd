; Minimal hand-written LLVM module for the runtime bridge experiment.
;
; The important contract is that a Nerd-visible main binding is emitted as
; @"$main". The C epilogue calls $main(), and clang lowers that reference to
; the same LLVM symbol.

define void @init() {
entry:
  ret void
}

define i32 @"$main"() {
entry:
  ret i32 0
}

