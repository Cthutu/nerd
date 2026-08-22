use std.files
use std.process

INPUT_FILE :: "return_2.c"

main :: fn () {
    preprocess_file(INPUT_FILE)
}

preprocess_file :: fn (input: string) {
    _ := run([
        "clang",
        "-E",
        input,
    ])
}

compile_file :: fn (input: string) {
    -- Compile the preprocessed file to assembly
    on read_text(input) => [file]
    {
        prn(file)
    }
}
¬
use std.files
use std.process

INPUT_FILE :: "return_2.c"

main :: fn () {
    preprocess_file(INPUT_FILE)
}

preprocess_file :: fn (input: string) {
    _ := run([
        "clang",
        "-E",
        input,
    ])
}

compile_file :: fn (input: string) {
    -- Compile the preprocessed file to assembly
    on read_text(input) => [file] {
        prn(file)
    }
}
