Failure :: enum {
    Bad
}

some :: fn () -> ?i32 {
    return 42
}

none :: fn () -> ?i32 {
    return nil
}

success :: fn () -> void\Failure {
}

failure :: fn () -> void\Failure {
    return Bad!
}

main :: fn () -> i32 {
    assert !no
    assert !!yes
    assert !none()
    assert !!some()
    assert !failure()
    assert !!success()
    return 0
}
¬
0
¬

¬
delete
