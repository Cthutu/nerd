# Desktop integration

`use std.system` provides synchronous desktop services on Windows and Linux.
Process execution remains in `std.process`.

| Function | Behaviour |
| --- | --- |
| `open_file(title = "Open file", output_arena = temp_arena)` | Select one existing file. |
| `save_file(title = "Save file", suggested_path = "", output_arena = temp_arena)` | Select a destination with overwrite confirmation; does not create a file. |
| `select_folder(title = "Select folder", output_arena = temp_arena)` | Select one existing filesystem folder. |
| `open_browser(url)` | Open an HTTP or HTTPS URL in the default browser. |
| `open_path(path)` | Open a local file or directory with its default application. |
| `open_explorer(directory = ".")` | Open an existing directory in the file manager. |

Dialog functions return `string\SystemError`. Launch functions return
`void\SystemError`. Defaulted arguments use named syntax and parameter order.
For example:

```nerd
use std.system

main :: fn () -> i32 {
    path := open_file(title = "Choose an image") else [error] {
        eprn(error.show())
        return 1
    }
    prn(path)
    return 0
}
```

The returned path belongs to `output_arena`; the default temporary arena keeps
it until reset/restoration. Paths retain Unicode and whitespace. Cancellation
returns `SystemError.Cancelled`, separately from `Invalid`, `Unavailable`, and
`Failed { code }`. `error.show()` supplies a diagnostic description. Failed
codes are native Windows error values or raw Linux wait statuses; zero denotes
an invalid provider result or capture failure without a native code. Linux
exit 127 maps to `Unavailable`. Launch success means the request was accepted,
not that the application finished loading or the user viewed its content.

Call desktop services on the application's UI thread. Dialogs block until the
user selects or cancels. Linux launchers are waited for; in a minimal desktop
where xdg-open waits for its application, opening can also block for that
application's lifetime. No worker threads or callbacks are hidden in the API.
Keep returned arena storage alive while using its string.

Windows uses `os.windows` bindings to `GetOpenFileNameW`, `GetSaveFileNameW`,
`SHBrowseForFolderW` and `ShellExecuteExW`. File/folder activation completes before
the calling COM apartment is released, so short-lived console programs can launch
folders reliably. The declarations follow the [Windows SDK common-dialog layout](https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew).
Local paths are resolved against the current directory and use absolute native
Windows separators before shell launching; browser URLs retain their URI spelling.
Paths convert between UTF-8 and
UTF-16. COM is initialised as an apartment on the calling thread and balanced
on return; a thread already initialised as a multithreaded apartment reports
failure. Dialogs are currently unparented, with all file types visible. File
selection uses a 32768-unit UTF-16 buffer; folder selection uses the shell's
legacy MAX_PATH contract. Filters, multiple selection, window ownership and
portal-backed Linux dialogs are future additions.

Linux uses `os.linux` fork/exec, pipe and wait wrappers, with a temporary libc
`execvp` binding for PATH lookup and inherited environment. Install `xdg-utils`
for `xdg-open` and `zenity` for GTK desktop dialogs. A graphical desktop session
is required. A provider exit status 1 is interpreted as cancellation, including
providers that also use that status for display-startup failure.
Titles and paths are individual arguments, never shell commands. Relative file
paths are prefixed with `./` to prevent option or URI interpretation. Provider
stdout is captured through an anonymous pipe, capped at 1 MiB, and only the
final newline is removed. User filenames containing whitespace or newlines
retain those characters. Other non-zero provider statuses report failure.

The public API rejects embedded NULs and malformed UTF-8. Browser opening
accepts the lowercase `http://` and `https://` schemes. `open_path` is the
explicit operation for local documents. `open_explorer` checks filesystem
metadata and currently rejects symbolic links as well as non-directory paths.

Run the interactive smoke example explicitly:

```sh
nerd run examples/system/system.n -- file
nerd run examples/system/system.n -- save
nerd run examples/system/system.n -- folder
nerd run examples/system/system.n -- browser
nerd run examples/system/system.n -- explorer
```

Check selection, cancellation, overwrite confirmation, Unicode paths and the
unchanged working directory. Automated tests cover validation and Windows ABI
sizes, Windows UTF-16 conversion, and Linux provider arguments, path capture,
cancellation, missing providers, oversized output and failure cleanup. The
Linux provider tests use isolated fake executables and display no UI:

```sh
python3 build/test_std_system.py --nerd _bin/nerd-debug
```

Clipboard text, desktop notifications and revealing a selected file in its
containing folder would be useful later additions. Environment, working
directory and executable-path helpers should be added when an application
needs them, with clear ownership and process-global mutation rules.
