main::fn()=>on 2{0..3 as matched on matched==2=>matched+20 0..3=>10 10 as size on size==10=>size*10 else as size=>size+100}
¬
main :: fn () =>
    on 2 {
        0..3 as matched on matched == 2 => matched + 20
        0..3                            => 10
        10 as size on size == 10        => size * 10
        else as size                    => size + 100
    }
