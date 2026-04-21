default:
    just --list

build *args:
    uv run build/build.py {{args}}

build-release *args:
    uv run build/build.py -r {{args}}

run proj *args: (build proj)
    _bin/{{proj}}-debug {{args}}

run-release proj *args: (build-release proj)
    _bin/{{proj}} {{args}}

clean:
    rm -rf _*
    find tests -type f \( -name '*.ir' -o -name '*.c' -o -name '*.out' -o -name '*.format' -o -name '*.input.n' -o -name '*.lsp.in' -o -name '*.lsp.out' \) -delete
    rm -rf syntax/nerd-vscode/node_modules
    rm -rf syntax/nerd-vscode/out
    rm -f syntax/nerd-vscode/package-lock.json
    rm -f syntax/nerd-vscode/*.vsix

format:
    python3 build/format.py

test: 
    just run nerd test

test-release:
    just run-release nerd test

test-build:
    just run nerd build -v nerd-src/main.n

alias b := build
alias br := build-release
alias r := run
alias rr := run-release
alias c := clean
alias f := format
alias t := test
alias tr := test-release
alias tb := test-build

#
# Recipes for VS Code extension packaging / installation
#

version := "0.0.1"
ext_name := "nerd-language-" + version
src_dir  := "syntax/nerd-vscode"
vsix := "nerd-language-" + version + ".vsix"
ext_id := "matt-davies.nerd-language"
user_bin_dir := "~/.local/bin"
user_bin_nerd := user_bin_dir + "/nerd"

npm-install:
    cd {{src_dir}} && npm install --omit=optional --no-fund --no-audit

package: npm-install
    cd {{src_dir}} && npm run package

uninstall:
    -code --uninstall-extension {{ext_id}}

install:
    just format
    just build-release nerd
    mkdir -p {{user_bin_dir}}
    cp _bin/nerd {{user_bin_nerd}}
    just uninstall
    just package
    code --install-extension {{src_dir}}/{{vsix}} --force
