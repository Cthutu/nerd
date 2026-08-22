-- test-platform: linux
use std.files

run_checks :: fn () -> i32\FileError {
    on !exists("../../README.md") => return 1
    on !is_file("../../README.md") => return 2
    on !is_directory(".") => return 3
    on is_symlink("../../README.md") => return 4
    on exists("_missing_std_files_test_") => return 5
    on is_file("_missing_std_files_test_") => return 12

    info := file_info("../../README.md")?
    on info.file_name != "../../README.md" => return 13
    on info.kind != FileKind.File => return 14
    on info.size < 6 => return 15

    text := read_text("../../README.md")?
    on text.count < 6 || text[..6] != "# Nerd" => return 6

    output_path := "_std_files_round_trip.tmp"
    defer _ := remove(output_path)
    write_text(output_path, "first")?
    output := open(output_path, FileMode.Append)?
    output.write_all("-second".as([]u8))?
    output.close()?
    on output.close() {
        FileError.Invalid! => {}
        else               => return 7
    }
    round_trip := read_text(output_path)?
    on round_trip != "first-second" => return 8
    remove(output_path)?

    result := file_info("_missing_std_files_test_")
    on result {
        FileError.NotFound! => return 0
        else                => return 10
    }
}

main :: fn () -> i32 {
    return on run_checks() { status => status _! => 11 }
}
¬
0
¬

¬
delete
¬
--llvm
