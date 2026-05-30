main::fn()->i32{
return on{1<0=>0 1==1=>1 else=>2}
}

score::fn(value:i32)->i32{
return on value{<0=>-1 ==0=>0 >=10=>100 else=>20}
}
¬
main :: fn () -> i32 {
    return on {
        1 < 0   => 0
        1 == 1  => 1
        else    => 2
    }
}

score :: fn (value: i32) -> i32 {
    return on value {
        < 0    => -1
        == 0   => 0
        >= 10  => 100
        else   => 20
    }
}
