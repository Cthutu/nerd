default:
    @just --list

# Build and package the Windows release compiler with its modules and documentation.
package:
    python3 build/package.py
