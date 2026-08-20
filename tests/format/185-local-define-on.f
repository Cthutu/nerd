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
