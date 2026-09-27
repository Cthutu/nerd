-- A large aggregate receiver plus these locals puts the named loop index
-- at a slot-table growth boundary when the item slot is added.
QueueInfo :: plex {
 queueFlags u32
 queueCount u32
 reserved [4]u32
}
impl Display for QueueInfo {

    show :: fn (self: Self) -> string {
        flags := [1, 2, 4, 8, 16, 32, 64, 256, 1024]
        names := ["graphics", "compute", "transfer", "sparse", "protected",
                  "decode", "encode", "flow", "data graph"]
        capabilities := ""
        remaining    := self.queueFlags

        for i, flag in flags {
            bit := flag^.as(u32)
            on (remaining & bit) != 0 => {
                separator := on capabilities.count == 0 => "" else ", "

                capabilities = temp_arena.pr($"{capabilities}{separator}{names[i]}")
                remaining    = remaining ^ bit
            }
        }
        on remaining != 0 => {
            separator := on capabilities.count == 0 => "" else ", "

            capabilities = temp_arena.pr($"{capabilities}{separator}unknown flags ({remaining})")
        }
        on capabilities.count == 0 => capabilities = "no capabilities"
        plural := on self.queueCount == 1 => "" else "s"
        return temp_arena.pr($"{self.queueCount} queue{plural}: {capabilities}")
    }

}

main :: fn () {
    family: QueueInfo = {
        queueCount: 1
        queueFlags: 7
        ...
    }
    assert family.show() == "1 queue: graphics, compute, transfer"
    family.queueFlags = 1024
    assert family.show() == "1 queue: data graph"
    prn("Indexed loop slots remain valid")
}

¬
0
¬
Indexed loop slots remain valid

¬
delete
¬
-r
