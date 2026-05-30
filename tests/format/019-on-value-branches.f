-- Branches need to be on separate lines

-- Block-type statements (on/for) should be on separate line in expression functions

main::fn()=>on 2{0=>10 1=>10 else=>30}
¬
-- Branches need to be on separate lines

-- Block-type statements (on/for) should be on separate line in expression
-- functions

main :: fn () =>
    on 2 {
        0     => 10
        1     => 10
        else  => 30
    }
