name :: "world"
count :: 40 + 2
truth :: yes
greeting :: $"Hello, {name}! count={count}, ok={truth}"

make :: fn () -> string {
    return $"constant {name} {count}"
}

main :: fn () => ((greeting != "Hello, world! count=42, ok=yes") ||
                  (make() != "constant world 42")).as(i32)
¬
0
¬
¬
delete
¬
--llvm-backend
