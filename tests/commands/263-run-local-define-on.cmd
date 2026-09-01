inactive_return :: fn () -> i32 {
    on "missing_local_on_test" {
        return 99
    }
    return 0
}

main :: fn () -> i32 {
    result := 0

    on "local_on_test" {
        result = 40
    }
    on !"local_on_test" {
        this_branch_must_not_resolve()
    }

    on "missing_local_on_test" {
        neither_must_this_one()
    }
    on !"missing_local_on_test" {
        result += 2
    }

    return result - 42 + inactive_return()
}
¬
0
¬

¬
delete
¬
-Dlocal_on_test
