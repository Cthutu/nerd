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

default_direction :: fn () -> Direction {
    return on yes => NORTH else NO_DIRECTION
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
    prn(direction_name(direction_from_word("n")))
    prn(direction_name(default_direction()))
    return 0
}
¬0¬
north
north

¬¬
