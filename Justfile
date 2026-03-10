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

test: 
    #!/bin/bash
    just run-release nerd benchmark
    ./_output
    printf "\nReturn code: %d\n" $?

alias b := build
alias br := build-release
alias r := run
alias rr := run-release
alias c := clean
alias t := test

#
# Recipes for VS Code syntax installation
#

version := "0.0.1"
ext_name := "nerd-language-" + version
src_dir  := "syntax/nerd-vscode"
vsix := "nerd-language-" + version + ".vsix"
ext_id := "matt-davies.nerd-language"

package:
    cd {{src_dir}} && vsce package

uninstall:
    -code --uninstall-extension {{ext_id}}

install: uninstall package
    code --install-extension {{src_dir}}/{{vsix}}

