-- ordinary comments should combine when adjacent to each other because this
-- keeps prose wrapping normally
--| +---+---+
--| | 0 | 1 |
--| +---+---+
-- ordinary comments resume combining after a protected diagram comment
-- line has ended

main::fn()=>42
¬
-- ordinary comments should combine when adjacent to each other because this
-- keeps prose wrapping normally
--| +---+---+
--| | 0 | 1 |
--| +---+---+
-- ordinary comments resume combining after a protected diagram comment line has
-- ended

main :: fn () => 42
