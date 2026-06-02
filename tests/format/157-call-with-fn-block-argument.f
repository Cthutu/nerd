pixels_draw :: fn (layer: ^PixelLayer, frame: u32) {
    pixels := layer.paint({
                        x: 0
                        y: 0
                        width: layer.pixel_width()
                        height: layer.pixel_height()
    }, fn (pixels: []u32, width: u32, height: u32, stride: u32) {
        for y in [0 .. height] {
            row := ^pixels[y * stride]
            for x in [0 .. width] {
                row[x] = pixel_colour(x, y, frame)
            }
        }
    })

    bar_width := (frame % layer.pixel_width().as(u32)) + 1
    layer.fill(PixelRect {
        x: 0 y: 0 width: bar_width height: 3
    }
    , 0xffffffff)
}
¬
pixels_draw :: fn (layer: ^PixelLayer, frame: u32) {
    pixels := layer.paint({
        x      : 0
        y      : 0
        width  : layer.pixel_width()
        height : layer.pixel_height()
    }, fn (pixels: []u32, width: u32, height: u32, stride: u32) {
        for y in [0 .. height] {
            row := ^pixels[y * stride]
            for x in [0 .. width] {
                row[x] = pixel_colour(x, y, frame)
            }
        }
    })

    bar_width := (frame % layer.pixel_width().as(u32)) + 1
    layer.fill(PixelRect {
        x      : 0
        y      : 0
        width  : bar_width
        height : 3
    }, 0xffffffff)
}
