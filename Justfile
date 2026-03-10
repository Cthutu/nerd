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

