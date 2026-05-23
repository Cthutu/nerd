-- test-platform: linux
-- test-debug-source: 149-build-imported-debug-info-linux.input.n
-- test-debug-source: tests/mods/test/folder_mod/mod.n
-- test-debug-source: tests/mods/test/parts/body.n

use test.folder_mod
use test.parts

main :: fn () -> i32 {
    return answer() + part_answer()
}
¬
0
¬

¬
debug-info
¬

¬
build
