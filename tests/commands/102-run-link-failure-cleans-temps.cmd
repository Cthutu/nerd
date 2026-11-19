ffi "nerd_missing_link_cleanup_test_library" {
    missing_link_symbol () -> i32
}

-- Reference the symbol so this remains a genuine linker-failure test.
main :: fn () -> i32 { return missing_link_symbol() }
¬
1
¬

¬
delete
¬
