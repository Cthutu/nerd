use core
use std.frame
use std.time

draw :: fn (_value: f64) {}

main :: fn () {
    frame_system := FrameSystem.init()
    frame := {
        system      : ^frame_system
        id          : NEW_FRAME
        width       : 800
        height      : 600
        title       : "Shadow"
        full_screen : no
        resizable   : yes
    }

    time := now()
    for frame_system.loop() {
        on frame_system.poll(^frame) {
            None => {
                time := now()
                draw(secs(time))
            }
            Closed => {
            }
            Resized { width, height } => {
                draw((width + height).as(f64))
            }
            KeyPress { scan_code: _ } => {
            }
            KeyRelease { scan_code: _ } => {
            }
            Character { codepoint: _ } => {
            }
        }
    }
}
¬
1
¬

¬
delete
¬

¬
check
¬
error: Unused local variable `time`
 --> 224-check-on-branch-shadowed-local.input.n:19:5
   |
17 |     }
18 | 
19 |     time := now()
   |     ^^^^ This local variable is never read
20 |     for frame_system.loop() {
21 |         on frame_system.poll(^frame) {
   |
note: Assigning to a variable does not count as using it.
help: Remove `time` or prefix the name with `_` if it is deliberately unused.
