use std.io

main::fn(){text::"hello" whole:string=text[..] ell:string=text[1..4] prn($"{ell.count} {ell.data[0]}")}
¬
use std.io

main :: fn () {
    text  :        : "hello"
    whole : string = text[..]
    ell   : string = text[1..4]

    prn($"{ell.count} {ell.data[0]}")
}
