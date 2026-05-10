use std.io

Rect :: plex {
    width  i32
    height i32
}

impl Rect {

    area :: fn (self: Self) -> i32 {
        return self.width * self.height
    }

    grow :: fn (self: ^Self, amount: i32) {
        self.width += amount
        self.height += amount
    }

}

use_methods :: fn (rect: ^Rect) -> i32 {
    rect.grow(2)
    return rect.area()
}

main :: fn () -> i32 {
    rect: Rect
    rect.grow(3)
    result := use_methods(^rect)
    prn($"area {result}")
    return 0
}
¬
0
¬
area 25
¬
delete
¬
--llvm-backend
