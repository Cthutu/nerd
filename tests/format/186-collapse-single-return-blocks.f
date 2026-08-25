choose :: fn (condition: bool) -> i32 {
    on condition => {
        return 1
    } else {
        return 2
    }
}

identity :: fn (value: i32) -> i32 {
    return value
}

contextual :: fn () -> ?i32 {
    return nil
}

kept :: fn (value: i32) -> i32 {
    inspect(value)
    return value
}

commented :: fn (value: i32) -> i32 {
    -- Keep this explanation.
    return value
}

guards :: fn (value: i32) -> i32 {
    on value < 0 => {
        return -1
    }
    on value == 0 => {
        return 0
    }
    on value > 100 => {
        return 100
    }
    return value
}
¬
choose :: fn (condition: bool) -> i32 {
    on condition => return 1 else return 2
}

identity :: fn (value: i32) => value

contextual :: fn () -> ?i32 {
    return nil
}

kept :: fn (value: i32) -> i32 {
    inspect(value)
    return value
}

commented :: fn (value: i32) -> i32 {
    -- Keep this explanation.
    return value
}

guards :: fn (value: i32) -> i32 {
    on value < 0   => return -1
    on value == 0  => return 0
    on value > 100 => return 100
    return value
}
