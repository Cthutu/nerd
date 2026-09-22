# Native Windows manual checks

Compiler implementation: `c760a1bf`; debugger harness: `abee4399`.
Both compiler binaries match the prepared desktop matrix hashes.

| Check | Status | Evidence / notes |
| --- | --- | --- |
| Pixels drawing, input/resize, normal exit | PENDING | Desktop matrix observations requested; builds passed. |
| Dungeon draws before input, accepts movement, Q exits | PENDING | Desktop matrix observations requested; builds passed. |
| Triangle drawing, input/resize, normal exit | PENDING | Desktop matrix observations requested; builds passed. |
| VS Code/CodeLLDB breakpoint, stepping, locals, call stack | PASS (user-reported smoke) | User ran launch-editor.ps1 and replied “It seemed to work” to the requested breakpoint/stepping/locals/call-stack procedure. Broad confirmation, not a detailed per-operation transcript. Automated debugger probes provide exact line/locals checks. |
| Generated C display/input/exit parity | PENDING | Desktop matrix includes LLVM and C, both compilers, both target modes and jobs 1/4. |

The isolated VS Code extension-development window used the freshly built
`_bin/nerd-debug.exe`, the local compiled extension, installed CodeLLDB 1.12.2,
and the validation LLVM/SDK environment. Global compiler and extensions were
not replaced. See `launch-editor.ps1`, `editor-integrations.log` and
`adapter-transforms-rerun.log`. Generated editor files remain under ignored scratch.

Desktop preparation/observation commands:

```powershell
. ./_tmp/windows-env.ps1
python validation/windows/manual.py validation/windows/results/20260922T073444Z-2a8ab0b9 --prepare
python validation/windows/manual.py validation/windows/results/20260922T073444Z-2a8ab0b9 --observe
```

The 48-case build/observation manifest is
[desktop.json](../20260922T073444Z-2a8ab0b9/desktop.json).
Do not infer visual correctness from successful builds or process lifetime.
