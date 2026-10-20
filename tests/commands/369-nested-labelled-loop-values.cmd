main :: fn () {
    direct := for $outer {
        for { break $outer 42 }
    }
    assert direct == 42

    deep := for $outer {
        for {
            for { break $outer 43 }
        }
    }
    assert deep == 43

    log := 0
    cleaned := for $outer {
        defer log = log * 10 + 1
        for {
            defer log = log * 10 + 2
            break $outer 44
        }
    }
    assert cleaned == 44
    assert log == 21

    own_values := for $outer {
        inner := for { break "inner" }
        assert inner == "inner"
        for { break $outer 45 }
    }
    assert own_values == 45

    shadowed := for $same {
        inner := for $same { break $same "inner" }
        assert inner == "inner"
        break $same 46
    }
    assert shadowed == 46

    block_shadow := for $same {
        inner := $same { break $same "block" }
        assert inner == "block"
        break $same 47
    }
    assert block_shadow == 47

    through_block := for $outer {
        $inner {
            for { break $outer 48 }
        }
    }
    assert through_block == 48

    scoped := for $outer {
        {
            for { break $outer 49 }
        }
    }
    assert scoped == 49

    finite := for i in [0 .. 3] $outer {
        for {
            on i == 2 => break $outer 50
            break
        }
    } else { break -1 }
    assert finite == 50

    exhausted := for i in [0 .. 0] $outer {
        for { break $outer i }
    } else { break 51 }
    assert exhausted == 51

    c_style := for i := 0; i < 3; i += 1 $outer {
        for {
            on i == 2 => break $outer 52
            break
        }
    } else { break -1 }
    assert c_style == 52

    inner_else := for $outer {
        for no {} else { break $outer 53 }
    }
    assert inner_else == 53

    wide: i64 = for $outer {
        for { break $outer 5_000_000_000 }
    }
    assert wide == 5_000_000_000

    function_boundary := for $same {
        local :: fn () -> string {
            return for $same { break $same "function" }
        }
        assert local() == "function"
        break $same 54
    }
    assert function_boundary == 54

    sibling := for $outer {
        for {
            on yes => break $outer 55
            break
        }
        break $outer 56
    }
    assert sibling == 55
}
¬
0
¬
