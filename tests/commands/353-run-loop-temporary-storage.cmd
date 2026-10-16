-- Each nested range and short-circuit expression has a short lifetime. Repeating
-- them must reuse fixed stack storage, including when no optimiser runs.
main :: fn () -> i32 {
    count: i32 = 0
    for iteration in [0 .. 1_000_000] {
        on iteration >= 0 && iteration < 1_000_000 => {
            for _ in [0 .. 2] {
                count += 1
            }
        }
    }
    assert count == 2_000_000
    return 0
}
¬
0
¬

¬
delete
¬
