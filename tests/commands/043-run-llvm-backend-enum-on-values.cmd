use std.io

Direction :: enum {
    NORTH
    EAST
    SOUTH
    WEST
    NO_DIRECTION
}

direction_from_word :: fn (word: string) -> Direction {
    return on word {
        "north", "n" => NORTH
        "east",  "e" => EAST
        "south", "s" => SOUTH
        "west",  "w" => WEST
        else => NO_DIRECTION
    }
}

direction_name :: fn (direction: Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        else => "none"
    }
}

main :: fn () -> i32 {
    Choice :: enum { Left(i32) Right(i32) }
    choice: Choice = Right(5)
    extra := on choice {
        Left(as n) => n
        Right(as n) => n
    }

    prn(direction_name(direction_from_word("n")))
    prn($"extra = {extra}")
    return extra
}
¬
5
¬
north
extra = 5

¬
delete
¬
--llvm-backend
