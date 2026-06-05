main::fn(){on x{0=>return on y{0=>a() 1=>b() else=>c()} 1=>{on z{0=>return on q{0=>d() else=>e()} 1=>f()}}}}
¬
main :: fn () {
    on x {
        0 => return on y {
            0     => a()
            1     => b()
            else  => c()
        }
        1 => {
            on z {
                0 => return on q {
                    0     => d()
                    else  => e()
                }
                1 => f()
            }
        }
    }
}
