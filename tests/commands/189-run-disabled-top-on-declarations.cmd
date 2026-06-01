on "missing_platform_key" {
    hidden :: fn (value: MissingType) -> AlsoMissing {
        return value
    }

    impl MissingReceiver {
        ignored :: fn (self: ^Self) -> MissingReturn {
            return self
        }
    }
}

main :: fn () -> i32 {
    return 7
}
¬
7
¬

