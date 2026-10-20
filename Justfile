default:
    @just --list

# Package the release compiler, VS Code extension, modules, examples, and docs.
package:
    python3 build/package.py
