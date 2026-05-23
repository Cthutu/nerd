-- test-platform: linux
-- test-debug-source: 149-build-imported-debug-info-linux.input.n
-- test-debug-source: tests/mods/test/folder_mod/mod.n

use test.folder_mod

main :: fn () -> i32 {
    return answer()
}
¬
0
¬

¬
debug-info
¬

¬
build
