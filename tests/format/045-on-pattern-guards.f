main::fn()=>on 2{matched@0..3 on matched==2=>matched+20 0..3=>10 exact@_ on exact==10=>exact*10 other@else=>other+100}
¬
main :: fn () =>
    on 2 {
        matched @ 0..3 on matched == 2 => matched + 20
        0..3                           => 10
        exact @ _ on exact == 10       => exact * 10
        other @ else                   => other + 100
    }
