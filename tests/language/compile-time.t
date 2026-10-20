=== case: 243-run-compile-time-parameter ===
choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 7 else 9
}

main :: fn () -> i32 {
    return choose() + choose(enabled = no) - 16
}
¬
0
¬

¬

¬

=== case: 104-location-macros ===
file_name :: @file
line_number :: @line

main :: fn () -> i32 {
    on file_name.count == 0 => return 1
    return (line_number - 2).as(i32)
}
¬
0
¬

¬

¬

=== case: 181-default-location-macros ===
capture_file :: fn (file: string = @file) => file
capture_line :: fn (line: u32 = @line) => line

main :: fn () -> i32 {
    on capture_file() != @file => return 1
    line := capture_line()
    on line != 6 => return line.as(i32)
    return 0
}
¬
0
¬

¬

¬

=== case: 100-run-unknown-pragmas ===
pragma unknown
pragma ignored(123, 4.5, "text", yes, no)

main :: fn () => 0
¬
0
¬

¬

¬

=== case: 207-run-embed-static-slice ===
use test.embed_forms

main :: fn () -> i32 {
    on static_bytes.count != 5 => return 1
    return static_total() - 326
}
¬
0
¬

¬

¬

=== case: 208-run-embed-inferred-slice ===
use test.embed_forms

main :: fn () -> i32 {
    return inferred_total() - 68
}
¬
0
¬

¬

¬

=== case: 209-run-embed-source-relative ===
use test.embed_relative

main :: fn () -> i32 {
    return local_total() - 24
}
¬
0
¬

¬

¬

=== case: 190-release-embedded-png-image ===
-- test-release
use std.image

main :: fn () -> i32 {
    bytes := @embed("../commands/release-demo.png")
    result := Image.load_bytes(bytes, desired_channels = 4)

    on result => [image] {
        on image.width != 320 => return 1
        on image.height != 200 => return 2
        on image.channels != 4 => return 3
        on image.data.count != (320 * 200 * 4) => return 4
        image.free()
        return 0
    } else {
        return 5
    }

    return 6
}
¬
0
¬

¬

¬


=== case: 241-release-embedded-jpeg-image ===
-- test-release
use std.image

main :: fn () -> i32 {
    bytes := @embed("../../examples/image_viewer/assets/demo.jpg")
    result := Image.load_bytes(bytes, desired_channels = 4)

    on result {
        image => {
            on image.width != 320 => return 1
            on image.height != 200 => return 2
            on image.channels != 4 => return 3
            on image.data.count != (320 * 200 * 4) => return 4
            image.free()
            return 0
        }
        else => return 5
    }

    return 6
}
¬
0
¬

¬

¬
