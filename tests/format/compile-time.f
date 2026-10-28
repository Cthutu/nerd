file_name::@file
line_number:=@line
¬
file_name   :: @file
line_number := @line
¬
pragma windowed
pragma ignored(1,2.5,"text",yes,no)

main::fn()=>0
¬
pragma windowed

pragma ignored(1, 2.5, "text", yes, no)

main :: fn () => 0
¬
pragma windowed
pragma ignored(1,2.5,"text",yes,no)

main::fn()=>0
broken :: fn () {
    value :: Thing {
        ...
    }
}
¬
pragma windowed

pragma ignored(1, 2.5, "text", yes, no)

main :: fn () => 0

broken :: fn () {
    value :: Thing { ... }
}
¬
load::fn[T](value:^atomic[T],order::AtomicLoadOrder=SequentiallyConsistent)->T{return value^}
¬
load :: fn [T] (value : ^atomic[T],
                order :: AtomicLoadOrder = SequentiallyConsistent) -> T {
    return value^
}
¬
main :: fn () {
    on "windows" {
        windows_only()
    }

    on !"windows" {
        portable_fallback()
    }
}
¬
main :: fn () {
    on "windows" {
        windows_only()
    }

    on !"windows" {
        portable_fallback()
    }
}
