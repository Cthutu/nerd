Callback :: fn (pixels: []u32, width: u32, height: u32, stride: u32)

with_pixels :: fn (_callback: Callback) {
}

pixel_colour :: fn (x: u32, y: u32) -> u32 {
    return x + y
}

main :: fn () {
    with_pixels(fn (pixels: []u32, width: u32, height: u32, stride: u32) {
        for y in [0 .. height] {
            for x in [0 .. width] {
                colour := pixel_colour(x, y)
                pixels[y * stride + x] = colour
            }
        }
    })
}
¬
0
¬

¬
delete
¬

¬
check
