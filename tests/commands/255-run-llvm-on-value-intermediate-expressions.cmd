Resolver :: fn () -> bool

platform_resolve :: fn () => yes

load_commands :: fn (resolve: Resolver = platform_resolve) -> bool {
    return resolve()
}

initialise :: fn (context: ?i32) -> bool {
    return on context {
        value => {
            on value == 0 => return no
            on !load_commands() => return no
            break yes
        }
        else => {
            break no
        }
    }
}

main :: fn () -> i32 {
    on !initialise(1) => return 1
    return 0
}
¬
0
¬
¬
delete
¬
--llvm
¬
run
¬
