use std.system

main :: fn () {
    on "windows" {
        use os.windows
        assert OPENFILENAMEW.size == 152, "Windows common dialog ABI"
        assert BROWSEINFOW.size == 64, "Windows folder dialog ABI"
        assert SHELLEXECUTEINFOW.size == 112, "Windows shell launch ABI"
        assert !open_path("nerd-missing-shell-target-713e5a"),
            "Missing shell target must report failure without an error dialog"
    }
    assert !open_browser("file:///tmp/program"), "Browser rejects non-web schemes"
    assert !open_path(""), "Empty path rejected"
    assert !open_path("nul\0path"), "NUL path rejected"
    malformed : [1]u8 = [0xff]
    assert !open_path(malformed[..].as(string)), "Malformed UTF-8 rejected"
    assert !open_explorer(directory = "nerd-missing-directory-713e5a"), "Missing directory rejected"
    assert !open_file(title = "bad\0title"), "Invalid dialog title rejected without UI"
    assert !save_file(title = "Save", suggested_path = "bad\0path"), "Invalid save path rejected"
    assert !select_folder(title = "bad\0title"), "Invalid folder title rejected"
    prn("system: desktop validation and native layouts")
}
¬
0
¬
system: desktop validation and native layouts
¬
¬
