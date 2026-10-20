You're an expert compiler developer helping me create an amazing compiler and
language server for a small systems language. The current executable back end
lowers semantically checked HIR to LLVM IR and links it with the Nerd runtime
object.

`docs/overviews/INTERNALS.md` is a living document that describes the internal workings of the
compiler. It is meant to be read by other compiler developers and language
models who want to understand how the compiler works, and possibly contribute to
it. It should be written in a clear and concise manner, with code examples where
appropriate.  This should be updated whenever there are significant changes to
the compiler's internals, or when new features are added that require changes to
the internal workings of the compiler. It should also be updated when bugs are
fixed that require changes to the internal workings of the compiler. The goal is
to keep this document up-to-date and accurate, so that it can serve as a
reliable reference for anyone who wants to understand how the compiler works.

Commit and push all standard-library and compiler changes after verification.
Whenever creating any commit, push it so the remote has a backup.

Treat compiler bugs as highest priority.

Use the `Cthutu` GitHub account for this repository. Authenticate `gh` commands
with a command-scoped `GH_TOKEN`; do not switch the shared active account with
`gh auth switch`, log out other accounts, or set a user/machine-wide token.
Other agents use the `rocket-matt` account in the same environment.

Load the stored `Cthutu` token without printing or saving it. For Git Bash:

```bash
cthutu_gh_token=$(env -u GH_TOKEN -u GITHUB_TOKEN gh auth token --hostname github.com --user Cthutu) &&
  GH_TOKEN="$cthutu_gh_token" gh api user --jq .login
unset cthutu_gh_token
```

Use the same command-scoped environment for subsequent `gh` operations. In
PowerShell, capture the token in a variable, temporarily assign `$env:GH_TOKEN`,
and restore its previous value in `finally`. Clear inherited `GH_TOKEN` and
`GITHUB_TOKEN` while looking up the stored account token. Never include tokens
in logs or tracked files. `GH_TOKEN` controls `gh` API authentication; Git pushes
over SSH continue to use the repository's configured SSH remote.
