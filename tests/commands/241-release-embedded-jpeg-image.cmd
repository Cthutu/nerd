use std.image

main :: fn () -> i32 {
    bytes := @embed("../../examples/image_viewer/assets/demo.jpg")
    result := Image.load_bytes(bytes, 4)

    on result {
        Ok(image) => {
            on image.width != 320 => return 1
            on image.height != 200 => return 2
            on image.channels != 4 => return 3
            on image.data.count != (320 * 200 * 4) => return 4
            image.free()
            return 0
        }
        else => return 5
    }

    return 6
}
¬
0
¬

¬
delete
¬
-r
