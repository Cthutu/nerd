-- test-platform: windows
use std.files

run_checks :: fn () -> i32\FileError {
    on !exists("../../README.md") => return 1
    on !is_file("../../README.md") => return 2
    on !is_directory(".") => return 3
    on exists("_missing_std_files_test_") => return 4
    on is_file("_missing_std_files_test_") => return 11

    info := file_info("../../README.md")?
    on info.file_name != "../../README.md" => return 12
    on info.kind != FileKind.File => return 13
    on info.size < 6 => return 14

    text := read_text("../../README.md")?
    on text.count < 6 || text[..6] != "# Nerd" => return 5

    output_path := "_std_files_round_trip.tmp"
    defer _ := remove(output_path)
    write_text(output_path, "first")?
    output := open(output_path, FileMode.Append)?
    output.write_all("-second".as([]u8))?
    output.close()?
    on output.close() {
        FileError.Invalid! => {}
        else               => return 6
    }
    round_trip := read_text(output_path)?
    on round_trip != "first-second" => return 7
    remove(output_path)?

    result := file_info("_missing_std_files_test_")
    on result {
        FileError.NotFound! => return 0
        else                => return 9
    }
}

main :: fn () -> i32 {
    return on run_checks() { status => status _! => 10 }
}
¬
0
¬

¬
delete
¬
--llvm
