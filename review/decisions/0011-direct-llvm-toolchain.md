# 0011: Direct LLVM Toolchain

Status: accepted
Date: 2026-09-19
Supersedes: [0009](0009-backend-toolchain-contract.md)

## Context

Matt requires the installed Nerd compiler to depend on LLVM tooling, without
invoking Clang. C output is a compatibility path, not the primary binary backend.
This is an architectural requirement rather than a claimed performance win.

## Decision

Continue rendering and combining textual LLVM IR. Invoke `opt` for release IR
optimisation, `llc` for native objects, and the host LLVM linker (`ld.lld`,
`lld-link`, or `ld64.lld`) for binaries. Use LLVM archive tools for static
libraries. Nerd owns the explicit host runtime and startup configuration.

Provide `nerd doctor` to report missing tools and SDK configuration, and verify
the executable pipeline through a temporary compile/link/run probe.

## Consequences

The installed compiler never invokes Clang. Clang remains a development tool for
building Nerd's C implementation and testing optional generated C. Host SDK
libraries remain required; LLVM tools do not replace the operating system CRT.
Tool failures report the exact command and preserve build intermediates.

The Linux implementation is validated with LLVM 22.1.8. Native Windows and macOS
validation remains necessary. Details and requirements are in
[toolchain setup](../../docs/toolchain.md).

## Follow-up

Rebaseline performance before the task-scheduler experiment; old Clang timings
cannot establish performance of the new pipeline. Cross-compilation remains
separate target-layout work.
