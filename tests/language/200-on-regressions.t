-- Runtime on regressions. Original IDs and exact golden output keep cases traceable.
-- HIR/LLVM-specific on fixtures remain separate.
use std.io

-- 129-run-on-expression-return-branches
read :: fn (value: ?u8) -> u8 {
    return on value {
        byte => {
            copied : u8 = byte
            return copied
        }
        else => {
            return 0
        }
    }
}

case_129 :: fn () -> i32 {
    result := read('x')
    prn($"{result}")
    return result.as(i32)
}

-- 172-run-on-enum-payload-branch-return
Size :: plex {
    width u16
    height u16
}

Mode :: enum {
    Fit {
        scale u16
    }

    Fixed {
        width u16
        height u16
    }
}

dims :: fn (mode: Mode) -> Size {
    on mode {
        Fit { scale } => {
            return Size { width: scale height: scale }
        }
        Fixed { width, height } => {
            return Size { width: width height: height }
        }
    }

    return Size { width: 1 height: 1 }
}

case_172 :: fn () {
    size := dims(Mode.Fixed { width: 10 height: 20 })
    prn($"{size.width}x{size.height}")
}

-- 221-run-on-expression-return-option-variant
direct :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => return nil
    }
    return byte
}

block :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => {
            return nil
        }
    }
    return byte
}

case_221 :: fn () -> i32 {
    direct_result := direct(nil)
    block_result := block(nil)

    on direct_result => {
        return 1
    } else {
    }

    on block_result => {
        return 2
    } else {
    }

    return 0
}

-- 222-run-on-expression-assignment-binder-read
out : u8

maybe :: fn () -> ?u8 {
    return 7
}

case_222 :: fn () -> i32 {
    out = on maybe() {
        program => program
        else => 0
    }
    prn($"{out}")
    return out.as(i32)
}

-- 227-run-on-expression-block-break-option
make :: fn (ok: bool) -> ?i32 {
    on ok => {
        value := 42
        return value
    }
    return nil
}

case_227 :: fn () -> i32 {
    result := make(yes)
    on result {
        value => {
            on (value != 42) => return 1
            return 0
        }
        else => return 2
    }
}

-- 231-run-on-pattern-same-name-different-payload
Payload231 :: plex {
    code i32
}

Thing231 :: enum {
    Number(u16)
    Record(Payload231)
}

case_231 :: fn () -> i32 {
    value : Thing231 = Thing231.Number(0x1234)
    on value {
        Thing231.Number(value) => {
            on value == 0x1234 => return 0
            return 1
        }
        Thing231.Record(value) => {
            _ := value
            return 2
        }
    }
}

-- 232-run-on-pattern-binder-read-in-condition
Payload232 :: plex {
    width  u32
    height u32
}

Thing232 :: enum {
    Done(Payload232)
    Other
}

case_232 :: fn () -> i32 {
    value : Thing232 = Thing232.Done({ width: 1 height: 2 })

    on value {
        Thing232.Done(header) => {
            on header.width == 0 || header.height == 0 => return 1
            return 0
        }
        Thing232.Other => return 2
    }
}

-- 233-run-generic-on-pattern-binder-read-in-condition
Payload233 :: plex {
    width  u32
    height u32
}

case_233 :: fn () -> i32 {
    -- An earlier enum uses a structurally equivalent payload (Payload232).
    -- Canonicalising these wrappers must preserve optional/result semantics.
    optional: ?Payload233 = { width: 3 height: 4 }
    on optional => [present] {
        assert present.width == 3 && present.height == 4
    } else {
        assert no
    }
    value : Payload233\string = { width: 1 height: 2 }

    on value => [header] {
        on header.width == 0 || header.height == 0 => return 1
        return 0
    } else {
        return 2
    }
}

-- 234-run-on-expression-branch-binder-result
Payload234 :: plex {
    width  u32
    height u32
}

case_234 :: fn () -> i32 {
    value : Payload234\string = { width: 1 height: 2 }

    header := on value => [parsed_header] {
        break parsed_header
    } else {
        return 1
    }

    return (header.width + header.height).as(i32) - 3
}

-- 252-run-nested-partial-on-statement
case_252 :: fn () -> i32 {
    result := 0

    on yes {
        yes => {
            on yes => {
                result = 42
            }
        }
    }

    return result - 42
}

-- 255-run-llvm-on-value-intermediate-expressions
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

case_255 :: fn () -> i32 {
    on !initialise(1) => return 1
    return 0
}

-- 270-run-condition-on-in-expression-block
case_270 :: fn () -> i32 {
    value := 1
    return on value {
        1 => ${
            break on value < 2 => ${
                break 42
            }
            else ${
                break 0
            }
        }
        else => 0
    }
}

-- 278-run-implicit-on-result-binders
make_result :: fn (failed: bool) -> i32\string {
    on failed => return "fail"!
    return 21
}

read_result :: fn (value: i32\string) -> i32 {
    on value => {
        return value * 2
    } else {
        return value.count.as(i32)
    }
}

read_optional :: fn (value: ?i32) -> i32 {
    on value => {
        return value
    } else {
        assert value == nil
        return 0
    }
}

check_bool :: fn (flag: bool) {
    on flag => {
        assert flag
    } else {
        assert !flag
    }
}

case_278 :: fn () -> i32 {
    check_bool(yes)
    check_bool(no)
    return read_result(make_result(no)) +
           read_result(make_result(yes)) +
           read_optional(7) +
           read_optional(nil) - 53
}

-- 309-run-on-implicit-binder-mutation
ready: bool
step :: fn () {
    on ready => { ready = no } else { ready = yes }
}
flip :: fn [T] (initial: T) -> T {
    value := initial
    on value => { value = no } else { value = yes }
    return value
}
case_309 :: fn () {
    step()
    assert ready
    step()
    assert !ready
    local := no
    on local => { assert no } else { local = yes }
    assert local
    on local => { local = no }
    assert !local
    assert flip(no)
    assert !flip(yes)
    absent: ?i32 = nil
    on absent => { assert no } else { absent = 7 }
    on absent => { assert absent == 7 } else { assert no }
    result: i32\string = 42
    on result => { assert result == 42 } else { assert no }
}

main :: fn () {
    prn("begin 129-run-on-expression-return-branches")
    assert case_129() == 120
    prn("pass 129-run-on-expression-return-branches")

    prn("begin 172-run-on-enum-payload-branch-return")
    case_172()
    prn("pass 172-run-on-enum-payload-branch-return")

    prn("begin 221-run-on-expression-return-option-variant")
    assert case_221() == 0
    prn("pass 221-run-on-expression-return-option-variant")

    prn("begin 222-run-on-expression-assignment-binder-read")
    assert case_222() == 7
    prn("pass 222-run-on-expression-assignment-binder-read")

    prn("begin 227-run-on-expression-block-break-option")
    assert case_227() == 0
    prn("pass 227-run-on-expression-block-break-option")

    prn("begin 231-run-on-pattern-same-name-different-payload")
    assert case_231() == 0
    prn("pass 231-run-on-pattern-same-name-different-payload")

    prn("begin 232-run-on-pattern-binder-read-in-condition")
    assert case_232() == 0
    prn("pass 232-run-on-pattern-binder-read-in-condition")

    prn("begin 233-run-generic-on-pattern-binder-read-in-condition")
    assert case_233() == 0
    prn("pass 233-run-generic-on-pattern-binder-read-in-condition")

    prn("begin 234-run-on-expression-branch-binder-result")
    assert case_234() == 0
    prn("pass 234-run-on-expression-branch-binder-result")

    prn("begin 252-run-nested-partial-on-statement")
    assert case_252() == 0
    prn("pass 252-run-nested-partial-on-statement")

    prn("begin 255-run-llvm-on-value-intermediate-expressions")
    assert case_255() == 0
    prn("pass 255-run-llvm-on-value-intermediate-expressions")

    prn("begin 270-run-condition-on-in-expression-block")
    assert case_270() == 42
    prn("pass 270-run-condition-on-in-expression-block")

    prn("begin 278-run-implicit-on-result-binders")
    assert case_278() == 0
    prn("pass 278-run-implicit-on-result-binders")

    prn("begin 309-run-on-implicit-binder-mutation")
    case_309()
    prn("pass 309-run-on-implicit-binder-mutation")
    prn("completed 14 on cases")
}
¬
0
¬
begin 129-run-on-expression-return-branches
120
pass 129-run-on-expression-return-branches
begin 172-run-on-enum-payload-branch-return
10x20
pass 172-run-on-enum-payload-branch-return
begin 221-run-on-expression-return-option-variant
pass 221-run-on-expression-return-option-variant
begin 222-run-on-expression-assignment-binder-read
7
pass 222-run-on-expression-assignment-binder-read
begin 227-run-on-expression-block-break-option
pass 227-run-on-expression-block-break-option
begin 231-run-on-pattern-same-name-different-payload
pass 231-run-on-pattern-same-name-different-payload
begin 232-run-on-pattern-binder-read-in-condition
pass 232-run-on-pattern-binder-read-in-condition
begin 233-run-generic-on-pattern-binder-read-in-condition
pass 233-run-generic-on-pattern-binder-read-in-condition
begin 234-run-on-expression-branch-binder-result
pass 234-run-on-expression-branch-binder-result
begin 252-run-nested-partial-on-statement
pass 252-run-nested-partial-on-statement
begin 255-run-llvm-on-value-intermediate-expressions
pass 255-run-llvm-on-value-intermediate-expressions
begin 270-run-condition-on-in-expression-block
pass 270-run-condition-on-in-expression-block
begin 278-run-implicit-on-result-binders
pass 278-run-implicit-on-result-binders
begin 309-run-on-implicit-binder-mutation
pass 309-run-on-implicit-binder-mutation
completed 14 on cases

¬

¬
