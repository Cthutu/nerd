-- error-handling feature scenarios. Results and stdout are checked in order.
use std.io

-- 103-defer
case_0_cleanup_return :: fn () -> i32 {
    value := 1
    defer value = 9
    return value
}

case_0_main :: fn () -> i32 {
    total := case_0_cleanup_return()
    order := 0
    {
        defer order = order * 10 + 1
        defer order = order * 10 + 2
    }
    total += order
    prn($"order={order}")

    {
        defer total += 10
        defer {
            prn("deferred-module-call")
        }
        total += 1
    }
    prn($"after-block={total}")

    for i := 0; i < 3; i += 1 {
        defer total += 100
        on i == 0 => again
        on i == 1 => break
        total += 1000
    }
    prn($"after-loop={total}")
    return total
}

-- 190-optional-result-types
case_1_make_optional :: fn (present: bool) -> ?i32 {
    on present => return 7
    return nil
}

case_1_make_result :: fn (failed: bool) -> i32\string {
    on failed => return "failed"!
    return 41
}

case_1_propagate_optional :: fn (present: bool) -> ?i32 {
    value := case_1_make_optional(present)?
    return value + 1
}

case_1_propagate_result :: fn (failed: bool) -> i32\string {
    value := case_1_make_result(failed)?
    return value + 1
}

case_1_optional_value :: fn (value: ?i32) -> i32 {
    return on value => [present] {
        break present
    } else {
        break 0
    }
}

case_1_result_value :: fn (value: i32\string) -> i32 {
    return on value => [success] {
        break success
    } else [error] {
        assert error.count > 0
        break -1
    }
}

case_1_match_result :: fn (value: i32\string) -> i32 {
    return on value {
        41      => 41
        success => success
    } else {
        "failed" => -1
        else     => -2
    }
}

case_1_match_optional :: fn (value: ?i32) -> i32 {
    return on value {
        7       => 70
        present => present
    } else {
        break 0
    }
}

case_1_main :: fn () -> i32 {
    local_optional: ?i32 = 7
    local_result: i32\string = "local"!
    return case_1_optional_value(case_1_propagate_optional(yes)) +
           case_1_optional_value(case_1_propagate_optional(no)) +
           case_1_result_value(case_1_propagate_result(no)) +
           case_1_result_value(case_1_propagate_result(yes)) +
           case_1_match_optional(case_1_make_optional(yes)) +
           case_1_optional_value(local_optional) +
           case_1_result_value(local_result) +
           case_1_match_result(case_1_make_result(no)) +
           case_1_match_result(case_1_make_result(yes)) - 115
}

-- 201-lazy-fallback
case_2_calls : i32
case_2_fallbacks : i32

case_2_optional :: fn (present: bool) -> ?i32 {
    case_2_calls += 1
    on present => return 7
    return nil
}

case_2_result :: fn (success: bool) -> i32\string {
    case_2_calls += 1
    on success => return 11
    return "failed"!
}

case_2_fallback :: fn () -> i32 {
    case_2_fallbacks += 1
    return 42
}

case_2_recover :: fn [T] (value: ?T, default: T) -> T {
    return value \ default
}

case_2_recover_result :: fn [T] (value: T\string, default: T) -> T {
    return value \ default
}

case_2_early_return :: fn (present: bool) -> i32 {
    value := case_2_optional(present) \ ${ return 99 }
    return value
}

case_2_fallible_fallback :: fn () -> i32\string {
    value := case_2_optional(no) \ case_2_result(no)?
    return value
}

Case_2_Record :: plex { value i32 }

case_2_void_result :: fn (success: bool) -> void\string {
    on success => return
    return "failed"!
}

case_2_void_fallback :: fn () {
    case_2_fallbacks += 1
}

case_2_main :: fn () -> i32 {
    assert case_2_optional(yes) \ case_2_fallback() == 7
    assert case_2_result(yes) \ case_2_fallback() == 11
    assert case_2_calls == 2 && case_2_fallbacks == 0

    assert case_2_optional(no) \ case_2_fallback() == 42
    assert case_2_result(no) \ case_2_fallback() == 42
    assert case_2_calls == 4 && case_2_fallbacks == 2

    -- Chains are right associative and stop at the first available value.
    assert case_2_optional(yes) \ case_2_result(yes) \ case_2_fallback() == 7
    assert case_2_calls == 5 && case_2_fallbacks == 2
    assert case_2_optional(no) \ case_2_result(yes) \ case_2_fallback() == 11
    assert case_2_calls == 7 && case_2_fallbacks == 2
    assert case_2_result(no) \ case_2_optional(no) \ case_2_fallback() == 42
    assert case_2_calls == 9 && case_2_fallbacks == 3

    -- Fallback binds above multiplication, below prefix and postfix operators.
    assert case_2_optional(yes) \ 2 * 3 + 1 == 22
    assert case_2_optional(no) \ (2 * 3 + 1) == 7
    assert case_2_optional(no) \ -2 * 3 == -6
    assert case_2_result(no) \ case_2_fallback().as(i32) == 42

    empty : ?i32
    assert empty \ ${ break 23 } == 23
    assert case_2_early_return(yes) == 7
    assert case_2_early_return(no) == 99
    failed := case_2_fallible_fallback()
    on failed => { return 1 } else [error] { assert error == "failed" }

    assert case_2_recover[i32](nil, 13) == 13
    assert case_2_recover_result[i32]("bad"!, 17) == 17
    present : ?string = "present"
    absent : ?string
    assert present \ "default" == "present"
    assert absent \ "default" == "default"
    record : ?Case_2_Record
    recovered := record \ Case_2_Record { value: 19 }
    assert recovered.value == 19
    small : ?u8
    inferred := small \ 255
    assert inferred == 255
    _ := case_2_optional(yes) \ case_2_fallback() -- Explicitly discarding stays lazy.

    -- Extract exactly one layer, including explicitly left-grouped chains.
    inner : ?i32 = 29
    nested : ??i32 = inner
    unwrapped := nested \ inner
    assert unwrapped \ 0 == 29
    assert (nested \ inner) \ 0 == 29
    widened : i32\string = empty \ 31
    assert widened \ 0 == 31

    before := case_2_fallbacks
    case_2_void_result(yes) \ case_2_void_fallback()
    assert case_2_fallbacks == before
    case_2_void_result(no) \ case_2_void_fallback()
    assert case_2_fallbacks == before + 1
    optional_void : ?void
    optional_void \ case_2_void_fallback()
    assert case_2_fallbacks == before + 2
    return 0
}

-- 202-bare-else
case_3_calls : i32
case_3_failures : i32
case_3_operation :: fn (success: bool) -> i32\string {
    case_3_calls += 1
    on !success => return "failed"!
    return 7
}
case_3_optional :: fn (present: bool) -> ?i32 {
    on present => return 9
    return nil
}
case_3_identity :: fn (value: i32) => value

case_3_inspect :: fn (success: bool) -> i32\string {
    value := case_3_operation(success) else [error] {
        assert error == "failed"
        case_3_failures += 1
    }
    return value + 1
}

case_3_main :: fn () -> i32 {
    assert case_3_inspect(yes) \ 0 == 8
    assert case_3_calls == 1 && case_3_failures == 0
    assert case_3_inspect(no) \ 23 == 23
    assert case_3_calls == 2 && case_3_failures == 1

    -- Full on extraction keeps independent value and error branches.
    answer := on case_3_operation(yes) => [value] ${ break value + 1 }
              else [error] ${ assert error == "failed" break 42 }
    assert answer == 8
    recovered := on case_3_operation(no) => [value] ${ break value }
                 else [error] ${ assert error == "failed" break 42 }
    assert recovered == 42

    -- The enclosing on owns an unbraced branch's else.
    grouped := on yes => (case_3_optional(yes) else { return 1 }) else 3
    assert grouped == 9
    called := on yes => case_3_identity(case_3_optional(yes) else { return 2 }) else 3
    assert called == 9
    nested := on yes => ${
        value := case_3_optional(yes) else { return 3 }
        break value
    } else 3
    assert nested == 9
    assigned : i32
    assigned = case_3_optional(yes) else { return 4 }
    assert assigned == 9
    assert case_3_identity(case_3_operation(yes) else { return 5 }) == 7
    return 0
}

-- 203-bare-else-on-subject
case_4_calls : i32
case_4_requested :: fn () -> bool\string {
    case_4_calls += 1
    return case_4_calls == 2
}
case_4_failed :: fn () -> bool\string { return "failed"! }
case_4_failure_exit :: fn () -> i32 {
    for {
        on case_4_failed() else [error] {
            assert error == "failed"
            return 17
        } => break
    }
    return 1
}
case_4_absent :: fn () -> ?bool { return nil }
case_4_propagate_error :: fn () -> i32\string {
    on case_4_failed() else [error] { assert error == "failed" } => return 1
    return 2
}
case_4_propagate_absence :: fn () -> ?i32 {
    on case_4_absent() else {} => return 1
    return 2
}
case_4_main :: fn () -> i32 {
    for {
        on case_4_requested() else [error] {
            assert error == "failed"
            return 1
        } => break
    }
    assert case_4_calls == 2
    case_4_calls = 0
    for {
        on {
            (case_4_requested() else { return 2 }) => break
        }
    }
    assert case_4_calls == 2
    assert case_4_failure_exit() == 17
    assert (case_4_propagate_error() \ 42) == 42
    assert (case_4_propagate_absence() \ 43) == 43
    on (case_4_failed() \ no) => return 3
    matched : i32
    on (on no => 1 else 2) {
        2 => matched = 2
        else => return 4
    }
    assert matched == 2
    -- An unbraced branch reserves else for the enclosing on.
    untouched := on yes => case_4_requested() else case_4_requested()
    assert (untouched \ no) == no
    nested := on yes => on case_4_requested() else { return 5 } => 7 else 9 else 11
    assert nested == 9
    recovered := on yes => (case_4_requested() else { return 6 }) else no
    assert recovered == no
    return 0
}

-- 238-run-dynarray-result-pattern
case_5_main :: fn () -> i32 {
    bytes : [..]u8 = nil
    result : [..]u8\string = bytes

    on result {
        output => {
            status := output.count == 0
            output.free()
            on status => return 0
        }
        else => {
        }
    }

    return 1
}

-- 271-run-void-result-success
Case_6_Failure :: enum {
    Bad
}

case_6_implicit_success :: fn () -> void\Case_6_Failure {
}

case_6_explicit_success :: fn () -> void\Case_6_Failure {
    return
}

case_6_both_succeed :: fn () -> void\Case_6_Failure {
    case_6_implicit_success()?
    case_6_explicit_success()?
}

case_6_main :: fn () -> i32 {
    on case_6_both_succeed() => {
        return 0
    } else {
        return 1
    }
}

-- 272-run-negate-optional-result
Case_7_Failure :: enum {
    Bad
}

case_7_some :: fn () -> ?i32 {
    return 42
}

case_7_none :: fn () -> ?i32 {
    return nil
}

case_7_success :: fn () -> void\Case_7_Failure {
}

case_7_failure :: fn () -> void\Case_7_Failure {
    return Bad!
}

case_7_main :: fn () -> i32 {
    assert !no
    assert !!yes
    assert !case_7_none()
    assert !!case_7_some()
    assert !case_7_failure()
    assert !!case_7_success()
    return 0
}

-- 287-run-inferred-expression-propagation
Case_8_Failure :: enum {
    Bad
}

case_8_result_value :: fn (ok: bool) -> i32\Case_8_Failure {
    on ok => return 41
    return Bad!
}

case_8_optional_value :: fn (ok: bool) -> ?i32 {
    on ok => return 42
    return nil
}

case_8_forward_result :: fn (ok: bool) => case_8_result_value(ok)? + 1
case_8_forward_optional :: fn (ok: bool) => case_8_optional_value(ok)?

case_8_main :: fn () -> i32 {
    on case_8_forward_result(no) {
        _! => {
        }
        else => return 3
    }
    on !case_8_forward_optional(no) => {
    } else return 4
    on !case_8_forward_optional(yes) => return 5
    return on case_8_forward_result(yes) {
        value => value - 42
        _!    => 2
    }
}

-- 306-run-void-result-propagation
Case_9_Error :: enum { Failed }
case_9_steps: i32
case_9_cleanup: i32
case_9_operation :: fn (fail: bool) -> void\Case_9_Error {
    on fail => return Case_9_Error.Failed!
}
case_9_run :: fn (fail: bool) -> i32\Case_9_Error {
    defer case_9_cleanup += 1
    case_9_operation(fail)?
    case_9_steps += 1
    case_9_operation(no)?
    case_9_steps += 1
    return 42
}
case_9_main :: fn () {
    on case_9_run(no) => [value] { assert value == 42 } else { assert no }
    assert case_9_steps == 2 && case_9_cleanup == 1
    on case_9_run(yes) => { assert no } else [error] { assert error == Case_9_Error.Failed }
    assert case_9_steps == 2 && case_9_cleanup == 2
}

-- 356-run-error-channel-patterns
Case_10_Failure :: enum { Bad WithCode { code i32 } }
case_10_plain :: fn (value: Case_10_Failure) -> i32 {
    on value {
        Case_10_Failure.Bad => { return 1 }
        Case_10_Failure.WithCode { code } => { return code }
    }
}
case_10_result :: fn (value: i32\Case_10_Failure) -> i32 {
    on value {
        Case_10_Failure.Bad! => { return 1 }
        Case_10_Failure.WithCode { code }! => { return code }
        success => { return success }
    }
}
case_10_table :: fn (value: i32\Case_10_Failure) -> i32 {
    on value { success => { return success } } else { error => { return case_10_plain(error) } }
}
case_10_main :: fn () {
    assert case_10_plain(Case_10_Failure.Bad) == 1
    assert case_10_plain(Case_10_Failure.WithCode { code: 42 }) == 42
    assert case_10_result(Case_10_Failure.Bad!) == 1
    assert case_10_result(Case_10_Failure.WithCode { code: 42 }!) == 42
    assert case_10_result(7) == 7
    assert case_10_table(Case_10_Failure.Bad!) == 1
    assert case_10_table(Case_10_Failure.WithCode { code: 42 }!) == 42
    assert case_10_table(7) == 7
}

main :: fn () -> i32 {
    prn("begin 103-defer")
    {
        result := case_0_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 233
    }
    prn("begin 190-optional-result-types")
    {
        result := case_1_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 50
    }
    prn("begin 201-lazy-fallback")
    {
        result := case_2_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 202-bare-else")
    {
        result := case_3_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 203-bare-else-on-subject")
    {
        result := case_4_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 238-run-dynarray-result-pattern")
    {
        result := case_5_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 271-run-void-result-success")
    {
        result := case_6_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 272-run-negate-optional-result")
    {
        result := case_7_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 287-run-inferred-expression-propagation")
    {
        result := case_8_main()
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 306-run-void-result-propagation")
    {
        case_9_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    prn("begin 356-run-error-channel-patterns")
    {
        case_10_main()
        result := 0
        status := result.as(u32) & 255
        prn($"result {status}")
        assert status == 0
    }
    return 0
}
¬
0
¬
begin 103-defer
order=21
deferred-module-call
after-block=33
after-loop=233
result 233
begin 190-optional-result-types
result 50
begin 201-lazy-fallback
result 0
begin 202-bare-else
result 0
begin 203-bare-else-on-subject
result 0
begin 238-run-dynarray-result-pattern
result 0
begin 271-run-void-result-success
result 0
begin 272-run-negate-optional-result
result 0
begin 287-run-inferred-expression-propagation
result 0
begin 306-run-void-result-propagation
result 0
begin 356-run-error-channel-patterns
result 0

¬

¬


=== case: 173-core-option-result ===
first :: fn [T] (iter: T) -> ?i32
where T: Iterator[i32] {
    return nil
}

main :: fn () -> i32 {
    _value: ?i32 = 21
    _status: i32\string = 21
    return 42
}
¬
42
¬

¬
hir 0
¬

=== case: 326-optional-void-return ===
noop :: fn (calls: ^i32) { calls^ += 1 }
expression_success :: fn (calls: ^i32) -> ?void { return noop(calls) }
implicit_success :: fn () -> ?void {}
explicit_success :: fn () -> ?void { return }
fail :: fn () -> ?void { return nil }
chain :: fn (ok: bool, count: ^i32) -> ?void {
    defer count^ += 10
    implicit_success()?
    explicit_success()?
    on !ok => { fail()? }
    count^ += 1
}
main :: fn () -> i32 {
    calls: i32 = 0
    on expression_success(^calls) => {} else { return 5 }
    on calls != 1 => return 8
    missing: ?void
    on missing => { return 7 }
    count: i32 = 0
    on chain(yes, ^count) => {} else { return 1 }
    on count != 11 => return 2
    on chain(no, ^count) => { return 3 }
    on count != 21 => return 4
    prn("optional void works")
    return 0
}
¬
0
¬
optional void works

¬

¬

=== case: 327-undo-cleanup ===
Error :: enum { Failed }
step :: fn (ok: bool) -> ?void {
    on !ok => return nil
}
work :: fn (first: bool, second: bool, later: bool, log: ^i32) -> ?void {
    defer log^ = log^ * 10 + 9
    step(first)? undo log^ = log^ * 10 + 1
    step(second)?
    undo { log^ = log^ * 10 + 2 }
    step(later)?
}
forward :: fn (value: ?i32, log: ^i32) -> ?i32 {
    undo log^ += 1
    return value
}
result :: fn (ok: bool, log: ^i32) -> void\Error {
    undo log^ = log^ * 10 + 1
    defer log^ = log^ * 10 + 2
    undo log^ = log^ * 10 + 3
    on !ok => return (Error.Failed)!
}
result_chain :: fn (ok: bool, log: ^i32) -> void\Error {
    undo log^ = log^ * 10 + 4
    result(ok, log)?
}
nested :: fn (early: bool, log: ^i32) -> ?void {
    undo log^ = log^ * 10 + 1
    on yes => {
        undo log^ = log^ * 10 + 2
        on early => return nil
    }
    return nil
}
latest :: fn (log: ^i32) -> ?void {
    value := 1
    undo log^ = value
    value = 7
    return nil
}
looped :: fn (log: ^i32) -> ?void {
    for i := 0; i < 3; i += 1 {
        undo log^ = log^ * 10 + 1
        defer log^ = log^ * 10 + 2
        on i == 0 => again
        on i == 1 => break
    }
    return nil
}
main :: fn () -> ?void {
    log: i32 = 0
    on work(no, yes, yes, ^log) => return nil
    on log != 9 => return nil
    log = 0
    on work(yes, no, yes, ^log) => return nil
    on log != 19 => return nil
    log = 0
    on work(yes, yes, no, ^log) => return nil
    on log != 219 => return nil
    log = 0
    work(yes, yes, yes, ^log)?
    on log != 9 => return nil
    log = 0
    on forward(nil, ^log) => return nil
    on log != 1 => return nil
    _value := forward(42, ^log)?
    on log != 1 => return nil
    log = 0
    on result_chain(no, ^log) => return nil
    on log != 3214 => return nil
    log = 0
    on result_chain(yes, ^log) => {} else return nil
    on log != 2 => return nil
    log = 0
    on nested(yes, ^log) => return nil
    on log != 21 => return nil
    log = 0
    on nested(no, ^log) => return nil
    on log != 1 => return nil
    log = 0
    on latest(^log) => return nil
    on log != 7 => return nil
    log = 0
    on looped(^log) => return nil
    on log != 22 => return nil
    prn("Undo registration, failure cleanup and success paths passed")
}
¬
0
¬
Undo registration, failure cleanup and success paths passed

¬

¬

=== case: 276-run-propagate-call-once ===
Failure :: enum {
    Bad
}

make :: fn (calls: ^i32) -> i32\Failure {
    calls^ += 1
    return 7
}

forward :: fn (calls: ^i32) -> i32\Failure {
    value := make(calls)?
    return value
}

main :: fn () -> i32 {
    calls : i32 = 0
    result := forward(^calls)
    return on result {
        value => on value == 7 && calls == 1 { yes => 0 else => calls }
        _!    => 10
    }
}
¬
0
¬

¬

¬
