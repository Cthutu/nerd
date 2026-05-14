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
    value :: Thing {
        ...
    }
}
