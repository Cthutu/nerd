use std.io

main :: fn () {
    answer :: $answer {
        break $answer 41 + 1
    }
    word :: $word {
        break $word "labelled"
    }
    outer :: $outer {
        $inner {
            break $outer 99
        }
        break $outer 0
    }
    hits := 0
    $void {
        hits += 1
        break $void
    }

    prn($"answer = {answer}")
    prn($"word = {word}")
    prn($"outer = {outer}")
    prn($"hits = {hits}")
    return outer
}
¬
99
¬
answer = 42
word = labelled
outer = 99
hits = 1

¬
delete
¬
--llvm-backend
