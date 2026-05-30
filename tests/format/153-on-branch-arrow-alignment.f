main::fn(){a:=on value{Short=>1 MuchLonger=>2 else=>3}
b:=on value{One=>1 Two=>{work()} Three=>3 FourLong=>4}
c:=on {x>10=>1 x>2&&x<8=>2 else=>3}}
¬
main :: fn () {
    a := on value {
        Short       => 1
        MuchLonger  => 2
        else        => 3
    }
    b := on value {
        One => 1
        Two => {
            work()
        }
        Three     => 3
        FourLong  => 4
    }
    c := on {
        x > 10          => 1
        x > 2 && x < 8  => 2
        else            => 3
    }
}
