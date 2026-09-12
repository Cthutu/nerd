use std.memory
use std.text
use std.slice
use std.time

main :: fn () -> i32 {
    bytes := alloc(8)
    defer free(bytes)
    on alloc_size(bytes) != 8 => return 1
    parts := " alpha,beta ".trim().split(",")
    defer parts.free()
    on !parts[..].contains("beta") => return 2
    on !"�".utf8_validate() => return 3
    on 10.as(Instant).add(from_ns(5)) != 15 => return 4
    on from_ms(1500).to_secs() != 1 => return 5
    on from_ms(1500).secs() != 1.5 => return 6
    return 0
}
¬
0
¬

¬
delete
¬
