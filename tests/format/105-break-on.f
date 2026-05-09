find :: fn (values: [..]i32, needle: i32) -> i32 {
return for value in values {
break on value^ == needle => value^
} else {
break -1
}
}

labelled :: fn () -> i32 {
return $pick {
break $pick on 1 == 1 => 5
break $pick 9
}
}
¬
find :: fn (values: [..]i32, needle: i32) -> i32 {
    return for value in values {
        break on value^ == needle => value^
    } else {
        break -1
    }
}

labelled :: fn () -> i32 {
    return $pick {
        break $pick on 1 == 1 => 5
        break $pick 9
    }
}
