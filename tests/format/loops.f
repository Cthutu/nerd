main :: fn () {
for { return 7 }
}
¬
main :: fn () {
    for {
        return 7
    }
}
¬
main :: fn () {
i := 0
for i < 5 { i += 1 }
}
¬
main :: fn () {
    i := 0
    for i < 5 {
        i += 1
    }
}
¬
use std.io

main :: fn () {
for i := 0, step := 1; i < 5; total += step, i += 1 { prn($"C {i}") }
}
¬
use std.io

main :: fn () {
    for i := 0, step := 1; i < 5; total += step, i += 1 {
        prn($"C {i}")
    }
}
¬
main :: fn () {
for i := 0; i < 6; i += 1 { on i == 2 => again

on i == 5 => break

total += i }
}
¬
main :: fn () {
    for i := 0; i < 6; i += 1 {
        on i == 2 => again

        on i == 5 => break

        total += i
    }
}
¬
main :: fn () {
for i := 0; i < 5; i += 1 $outer { on i == 2 => again $outer
on i == 4 => break $outer }
value :: for $value { break $value 7 }
}
¬
main :: fn () {
    for i := 0; i < 5; i += 1 $outer {
        on i == 2 => again $outer
        on i == 4 => break $outer
    }
    value :: for $value {
        break $value 7
    }
}
¬
main :: fn () {
value :: for i := 0; i < 1; i += 1 { break i } else { break 99 }
}
¬
main :: fn () {
    value :: for i := 0; i < 1; i += 1 {
        break i
    } else {
        break 99
    }
}
¬
use std.io
main::fn()->i32{
values:[2]i32=[1,2]
ptr:^i32=^values[1]
for item in values[..]{
prn($"{item^}")
}
return ptr^
}
¬
use std.io

main :: fn () -> i32 {
    values : [2]i32 = [1, 2]
    ptr    : ^i32   = ^values[1]

    for item in values[..] {
        prn($"{item^}")
    }
    return ptr^
}
¬
main::fn(text:string,cond:bool){
value:=1
ptr:^i32=^value
for c in text{
c^ = 1
}
on cond=>return yes
}
¬
main :: fn (text: string, cond: bool) {
    value        := 1
    ptr   : ^i32 = ^value

    for c in text {
        c^ = 1
    }
    on cond => return yes
}
¬
main::fn(){for i,item in items{} for j,ptr in items{}}
¬
main :: fn () {
    for i, item in items {
    }
    for j, ptr in items {
    }
}
¬
FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {
find_frame_info :: fn (fs: ^Self, handle: u64)
{
return for frame in fs.frames {
on frame.handle == handle => frame
} else {
nil
}
}
}
¬
FrameInfo :: plex {
    handle u64
}

FrameSystem :: plex {
    frames [..]FrameInfo
}

impl FrameSystem {

    find_frame_info :: fn (fs: ^Self, handle: u64) {
        return for frame in fs.frames {
            on frame.handle == handle => frame
        } else {
            nil
        }
    }

}
¬
main::fn(a:u64,
-- keep parameter comment
b:

loop::fn(){
for index,item in values{
-- keep loop comment
item.count+=1
¬
main :: fn (a: u64,
-- keep parameter comment
b:

loop :: fn () {
    for index, item in values {
        -- keep loop comment
        item.count += 1
¬
main::fn(){for i in [0..10]{_:=i} for i in [2..=4]{_:=i}}
¬
main :: fn () {
    for i in [0 .. 10] {
        _ := i
    }
    for i in [2 ..= 4] {
        _ := i
    }
}
¬
main::fn(){for item in iter{} for i,item in iter{}}
¬
main :: fn () {
    for item in iter {
    }
    for i, item in iter {
    }
}
¬
main::fn(value:i32){on value in [0..=9]=>{for (value in [0..10]){break}} on value{in [0..=9]=>1
else=>2}}
¬
main :: fn (value: i32) {
    on value in [0 ..= 9] => {
        for (value in [0 .. 10]) {
            break
        }
    }
    on value {
        in [0 ..= 9] => 1
        else => 2
    }
}
¬
main::fn(input:string){offset:=0
for offset<input.count&&(input[offset]>=0){offset+=1}
for offset<input.count&&(is_lower(input[offset])||is_upper(input[offset])||input[offset]=='_'||is_digit(input[offset])){offset+=1}
for offset<input.count&&
input[offset]!='\0'{offset+=1}}
¬
main :: fn (input: string) {
    offset := 0
    for offset < input.count && (input[offset] >= 0) {
        offset += 1
    }
    for offset < input.count &&
        (is_lower(input[offset]) ||
            is_upper(input[offset]) ||
            input[offset] == '_' ||
            is_digit(input[offset])) {
        offset += 1
    }
    for offset < input.count &&
        input[offset] != '\0' {
        offset += 1
    }
}
¬
VulkanContext :: plex {
frame ^Frame
instance VkInstance
physical_device VkPhysicalDevice
}
choose :: fn () {
for device in devices {
properties: VkPhysicalDeviceProperties
vkGetPhysicalDeviceProperties(device^, ^properties)
}
for device in devices {
}
for device in devices $search {
properties: VkPhysicalDeviceProperties
use_properties(^properties)
}
}

¬
VulkanContext :: plex {
    frame           ^Frame
    instance        VkInstance
    physical_device VkPhysicalDevice
}

choose :: fn () {
    for device in devices {
        properties : VkPhysicalDeviceProperties
        vkGetPhysicalDeviceProperties(device^, ^properties)
    }
    for device in devices {
    }
    for device in devices $search {
        properties : VkPhysicalDeviceProperties
        use_properties(^properties)
    }
}
