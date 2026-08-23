main::fn(value:i32){on value in [0..=9]=>{for (value in [0..10]){break}} on value{in [0..=9]=>1
else=>2}}
¬
main :: fn (value: i32) {
    on value in [0 ..= 9] => {
        for (value in [0 .. 10]) {
            break
        }
    }
    on value {
        in [0 ..= 9] => 1
        else => 2
    }
}
