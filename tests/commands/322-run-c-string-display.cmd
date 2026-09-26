render :: fn [T] (value: T) -> string
where T: Display {
    return $"{value}"
}

main :: fn () -> i32 {
    text := c"Vulkan validation"
    empty := c""
    absent: c_string = nil
    utf8 := c"héllo"
    bytes: [4]i8 = [65, 0, 66, 0]
    terminated: c_string = bytes.data

    -- Exercise implicit core discovery before any direct show() call.
    on render(text) != "Vulkan validation" => return 1
    on $"{text}" != "Vulkan validation" => return 2
    on $"[{empty}][{absent}]" != "[][]" => return 3
    on $"{utf8}" != "héllo" => return 4
    on $"{terminated}" != "A" => return 5
    view := terminated.show()
    on view.data != bytes.data.as(^u8) => return 6
    on view.count != 1 => return 7
    on absent.show() != "" => return 8
    prn($"{text}: {utf8} [{terminated}]")
    return 0
}
¬
0
¬
Vulkan validation: héllo [A]

¬
delete
¬
