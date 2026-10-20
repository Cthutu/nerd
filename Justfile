default:
    @just --list

# Package the compiler, VS Code extension, LLVM Scoop manifest, and documentation.
package:
    python3 build/package.py
