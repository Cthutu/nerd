default:
    just --list

exe_suffix := if os_family() == "windows" { ".exe" } else { "" }

build *args:
    uv run build/build.py {{args}}

build-release *args:
    uv run build/build.py -r {{args}}

run proj *args: (build proj)
    ./_bin/{{proj}}-debug{{exe_suffix}} {{args}}

run-release proj *args: (build-release proj)
    ./_bin/{{proj}}{{exe_suffix}} {{args}}

clean:
    rm -rf _*
    find tests -type f \( -name '*.c' -o -name '*.out' -o -name '*.format' -o -name '*.input.n' -o -name '*.lsp.in' -o -name '*.lsp.out' \) -delete
    rm -rf syntax/nerd-vscode/node_modules
    rm -rf syntax/nerd-vscode/out
    rm -f syntax/nerd-vscode/package-lock.json
    rm -f syntax/nerd-vscode/*.vsix

format:
    python3 build/format.py

test *args:
    just build nerd
    python3 build/test.py {{args}}

test-release *args:
    just build-release nerd
    python3 build/test.py {{args}}

test-build:
    just run nerd build -v examples/text-adventure/adv.n

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
user_bin_dir := if os_family() == "windows" { replace(env_var("USERPROFILE"), "\\", "/") + "/.local/bin" } else { "~/.local/bin" }
user_bin_nerd := user_bin_dir + "/nerd" + exe_suffix
user_mods_dir := user_bin_dir + "/mods"
nvim_src_dir := "syntax/nerd-nvim"
nvim_config_dir := if os_family() == "windows" { replace(env_var("LOCALAPPDATA"), "\\", "/") + "/nvim" } else { "~/.config/nvim" }
nvim_plugin_dir := nvim_config_dir + "/lua/plugins"
nvim_ftdetect_dir := nvim_config_dir + "/ftdetect"
nvim_syntax_dir := nvim_config_dir + "/syntax"

npm-install:
    cd {{src_dir}} && npm install --omit=optional --no-fund --no-audit

package: npm-install
    cd {{src_dir}} && npm run package

uninstall:
    -code --uninstall-extension {{ext_id}}

install-nvim:
    mkdir -p {{nvim_plugin_dir}} {{nvim_ftdetect_dir}} {{nvim_syntax_dir}}
    cp {{nvim_src_dir}}/lua/plugins/nerd.lua {{nvim_plugin_dir}}/nerd.lua
    cp {{nvim_src_dir}}/ftdetect/nerd.vim {{nvim_ftdetect_dir}}/nerd.vim
    cp {{nvim_src_dir}}/syntax/nerd.vim {{nvim_syntax_dir}}/nerd.vim

install:
    just format
    just build-release nerd
    rm -rf {{user_mods_dir}}
    mkdir -p {{user_bin_dir}} {{user_mods_dir}}
    cp _bin/nerd{{exe_suffix}} {{user_bin_nerd}}.tmp
    mv {{user_bin_nerd}}.tmp {{user_bin_nerd}}
    cp -R mods/. {{user_mods_dir}}/
    just install-nvim
    just uninstall
    just package
    code --install-extension {{src_dir}}/{{vsix}} --force

test-install:
    just test
    just install

alias i := install
alias ti := test-install

do:
    just clean
    just test
    just test-release
    just install
