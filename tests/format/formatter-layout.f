-- This is a comment paragraph that should
--be wrapped by the formatter because it
-- is deliberately much longer than the fixed formatting width and should be reflowed into stable output lines.
--
-- Second paragraph stays separate.


main
 :
 : 
fn (

) => 
42
¬
-- This is a comment paragraph that should be wrapped by the formatter because
-- it is deliberately much longer than the fixed formatting width and should be
-- reflowed into stable output lines.
--
-- Second paragraph stays separate.

main :: fn () => 42
¬
-- Heading comment.



main :: 42



other :: fn () => main
¬
-- Heading comment.

main :: 42

other :: fn () => main
¬
main::fn()=>-(2-5)+1*  3
¬
main :: fn () => -(2 - 5) + 1 * 3
¬
main::fn()=>((1+2)+3)
¬
main :: fn () => ((1 + 2) + 3)
¬
a :: fn () -> i32 {
return 1
}
-- doc for b
b :: fn () -> i32 {
return 2
}
¬
a :: fn () -> i32 {
    return 1
}

-- doc for b
b :: fn () -> i32 {
    return 2
}
¬
use std.random
use std.collections
use std.io
use std.term

test "Basic" {
    assert 1 + 1 == 2, "Simple check"
    -- assert 1 + 0 == 2, "Simple fail"
}
¬
use std.collections
use std.io
use std.random
use std.term

test "Basic" {
    assert 1 + 1 == 2, "Simple check"
    -- assert 1 + 0 == 2, "Simple fail"
}
¬
main :: fn () {
    on yes => return


    setlocale(LC_CTYPE, c"")

    -- explain the next step
    after_comment := 2

    value := 1
}
¬
main :: fn () {
    on yes => return

    setlocale(LC_CTYPE, c"")

    -- explain the next step
    after_comment := 2

    value := 1
}
¬
use beta -- beta import
use alpha -- alpha import

pub Frame::plex {
    handle u64 -- native handle
    system ^FrameSystem -- owning system comment that is deliberately long enough to wrap onto another line while keeping the marker aligned
    title string -- visible title
}

main :: fn () {
    answer:=42 -- keep answer
    return -- return default
}

top::1 -- top comment
¬
use beta  -- beta import
use alpha  -- alpha import

pub Frame :: plex {
    handle u64           -- native handle
    system ^FrameSystem  -- owning system comment that is deliberately long
                         --     enough to wrap onto another line while keeping
                         --     the marker aligned
    title  string        -- visible title
}

main :: fn () {
    answer := 42  -- keep answer
    return  -- return default
}

top :: 1  -- top comment
¬
pub Frame::plex {
    handle u64 -- native handle
    system ^FrameSystem -- owning system comment that is deliberately long
                        --     enough to wrap onto another line while keeping the
                        --     marker aligned
    mini ^FrameSystem -- short
                        --     more
    title string -- visible title
}
¬
pub Frame :: plex {
    handle u64           -- native handle
    system ^FrameSystem  -- owning system comment that is deliberately long
                         --     enough to wrap onto another line while keeping
                         --     the marker aligned
    mini   ^FrameSystem  -- short more
    title  string        -- visible title
}
¬
main :: fn () {
    use beta
    -- keep use separator
    use alpha

    first:=1
    -- keep assignment separator
    second:=2

    left :: 1
    -- keep const separator
    right :: 2
}
¬
main :: fn () {
    use beta
    -- keep use separator
    use alpha

    first := 1
    -- keep assignment separator
    second := 2

    left :: 1
    -- keep const separator
    right :: 2
}
¬
main::fn(){
value:=
-- keep this near partial code
}
¬
main :: fn () {
    value :=
    -- keep this near partial code
}
¬
main::fn(){
items:=[
1,
2,
-- trailing item
]
on value{
1=>{
result:=
}
else=>{
fallback:=Person{
name:"matt",
age:
}
}
¬
main :: fn () {
    items := [
        1,
        2,
        -- trailing item
    ]
    on value {
        1 => {
            result :=
        }
        else => {
            fallback := Person {
                name: "matt",
                age:
            }
        }
¬
main::fn(){
answer:=sum(
1,
-- keep argument comment
}

use test.{
foo,
-- keep module comment
bar
¬
main :: fn () {
    answer := sum(
    1,
    -- keep argument comment
}

use test.{
    foo,
    -- keep module comment
    bar
¬
winsize::plex#c{
ws_row i16
ws_col i16
}

ffi "c" ioctl :: fn (fd: i32, request: i32, ...) -> i32

main :: fn () {
    prn("a" + "b"
}
¬
winsize :: plex #c {
    ws_row i16
    ws_col i16
}

ffi "c" ioctl :: fn (fd: i32, request: i32, ...) -> i32

main :: fn () {
    prn("a" + "b"
}
¬
main::fn(){
first:=1


-- lead for second


second:=2
-- adjacent comment without blank
third:=3


-- paragraph one


-- paragraph two
fourth:=4
}
¬
main :: fn () {
    first := 1

    -- lead for second

    second := 2
    -- adjacent comment without blank
    third := 3

    -- paragraph one

    -- paragraph two
    fourth := 4
}
¬
on "windows" {
    pub WS_TILED : DWORD : WS_OVERLAPPED
    pub WS_OVERLAPPEDWINDOW : DWORD : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
}
¬
on "windows" {

    pub WS_TILED            : DWORD : WS_OVERLAPPED
    pub WS_OVERLAPPEDWINDOW : DWORD : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                                          | WS_THICKFRAME | WS_MINIMIZEBOX
                                          | WS_MAXIMIZEBOX

}
¬
value::1 -- value comment

main::fn(){
    answer:=42 -- answer comment
    return -- return comment
}
¬
value :: 1  -- value comment

main :: fn () {
    answer := 42  -- answer comment
    return  -- return comment
}
¬
-- ordinary comments should combine when adjacent to each other because this
-- keeps prose wrapping normally
--| +---+---+
--| | 0 | 1 |
--| +---+---+
-- ordinary comments resume combining after a protected diagram comment
-- line has ended

main::fn()=>42
¬
-- ordinary comments should combine when adjacent to each other because this
-- keeps prose wrapping normally
--| +---+---+
--| | 0 | 1 |
--| +---+---+
-- ordinary comments resume combining after a protected diagram comment line has
-- ended

main :: fn () => 42
¬
Stats::plex {
    heap_bytes_reallocated usize -- Total bytes requested by heap reallocations.
    heap_bytes_freed usize -- Total bytes released by heap frees.
}
¬
Stats :: plex {
    heap_bytes_reallocated usize  -- Total bytes requested by heap
                                  --     reallocations.
    heap_bytes_freed       usize  -- Total bytes released by heap frees.
}
¬
main :: fn () {
for $repeat {
continue $repeat
}
}
¬
main :: fn () {
    for $repeat {
        continue $repeat
    }
}
¬
main :: fn () {
compress_error :: fn (error: CompressionError) -> [..]u8\CompressionError {
}

result:=Err(Error.InvalidData{
message
})

counts : [16]u16
next_codes : [16]u16

for i:usize=0;
i<lengths.count;
i+=1 {
    length:=lengths[i]
}

state:=on pushed {
Err(OutputLimitExceeded)=>return compress_bool_error(OutputLimitExceeded) else=>{}
}

deflate_decode_compressed_block :: fn (reader : ^DeflateReader,
output : ^[..]u8,
literals : ^Huffman,
distances : ^Huffman,
max_output_bytes : usize) -> bool\CompressionError {
}
}
¬
main :: fn () {
    compress_error :: fn (error: CompressionError) -> [..]u8\CompressionError {
    }

    result := Err(Error.InvalidData { message })

    counts     : [16]u16
    next_codes : [16]u16

    for i: usize = 0;
        i < lengths.count;
        i += 1 {
        length := lengths[i]
    }

    state := on pushed {
        Err(OutputLimitExceeded) => return compress_bool_error(OutputLimitExceeded)

        else => {
        }
    }

    deflate_decode_compressed_block :: fn (reader           : ^DeflateReader,
                                           output           : ^[..]u8,
                                           literals         : ^Huffman,
                                           distances        : ^Huffman,
                                           max_output_bytes : usize) -> bool\CompressionError {
    }
}
¬
choose :: fn (condition: bool) -> i32 {
    on condition => {
        return 1
    } else {
        return 2
    }
}

identity :: fn (value: i32) -> i32 {
    return value
}

contextual :: fn () -> ?i32 {
    return nil
}

kept :: fn (value: i32) -> i32 {
    inspect(value)
    return value
}

commented :: fn (value: i32) -> i32 {
    -- Keep this explanation.
    return value
}

guards :: fn (value: i32) -> i32 {
    on value < 0 => {
        return -1
    }
    on value == 0 => {
        return 0
    }
    on value > 100 => {
        return 100
    }
    return value
}
¬
choose :: fn (condition: bool) -> i32 {
    on condition => return 1 else return 2
}

identity :: fn (value: i32) -> i32 {
    return value
}

contextual :: fn () -> ?i32 {
    return nil
}

kept :: fn (value: i32) -> i32 {
    inspect(value)
    return value
}

commented :: fn (value: i32) -> i32 {
    -- Keep this explanation.
    return value
}

guards :: fn (value: i32) -> i32 {
    on value < 0   => return -1
    on value == 0  => return 0
    on value > 100 => return 100
    return value
}
¬
broken::fn(){
    -- Keep this comment and malformed address expression.
    call(&value)
}
debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
_messageType : VkDebugUtilsMessageTypeFlagsEXT,
callbackData : ^VkDebugUtilsMessengerCallbackDataEXT,
_userData : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
¬
broken :: fn () {
    -- Keep this comment and malformed address expression.
    call(& value)
}

debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
                      _messageType    : VkDebugUtilsMessageTypeFlagsEXT,
                      callbackData    : ^VkDebugUtilsMessengerCallbackDataEXT,
                      _userData       : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
¬
first_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
_messageType : VkDebugUtilsMessageTypeFlagsEXT,
callbackData : ^VkDebugUtilsMessengerCallbackDataEXT,
_userData : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
broken::fn(){
    -- Keep this comment and malformed address expression.
    call(&value)
}
debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
_messageType : VkDebugUtilsMessageTypeFlagsEXT,
callbackData : ^VkDebugUtilsMessengerCallbackDataEXT,
_userData : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
¬
first_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
                      _messageType    : VkDebugUtilsMessageTypeFlagsEXT,
                      callbackData    : ^VkDebugUtilsMessengerCallbackDataEXT,
                      _userData       : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}

broken :: fn () {
    -- Keep this comment and malformed address expression.
    call(& value)
}

debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
                      _messageType    : VkDebugUtilsMessageTypeFlagsEXT,
                      callbackData    : ^VkDebugUtilsMessengerCallbackDataEXT,
                      _userData       : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
¬
debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
_messageType : VkDebugUtilsMessageTypeFlagsEXT,
callbackData : ^VkDebugUtilsMessengerCallbackDataEXT,
_userData : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
broken::fn(){
    -- Keep this comment and malformed address expression.
    call(&value)
}
¬
debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
                      _messageType    : VkDebugUtilsMessageTypeFlagsEXT,
                      callbackData    : ^VkDebugUtilsMessengerCallbackDataEXT,
                      _userData       : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}

broken :: fn () {
    -- Keep this comment and malformed address expression.
    call(& value)
}
¬
pub
choose :: fn (value: i32) -> i32 =>
on value > 0 => 1 else 0
short :: 1
longer_name :: 2
broken :: fn () { call(&value) }
pub
other :: fn (value: i32) -> i32 =>
on value > 0 => 1 else 0
another :: 1
longest_name :: 2
¬
pub choose :: fn (value: i32) => on value > 0 => 1 else 0

short       :: 1
longer_name :: 2

broken :: fn () {
    call(& value)
}

pub other :: fn (value: i32) => on value > 0 => 1 else 0

another      :: 1
longest_name :: 2
¬
wrap :: fn (value: i32) -> i32\Error { return value }
optional :: fn (value: i32) -> ?i32 { return value }
widen :: fn (value: i32) -> i64 { return value }
call :: fn () -> i64 { return narrow() }
inferred :: fn (value: i32) => value
¬
wrap :: fn (value: i32) -> i32\Error {
    return value
}

optional :: fn (value: i32) -> ?i32 {
    return value
}

widen :: fn (value: i32) -> i64 {
    return value
}

call :: fn () -> i64 {
    return narrow()
}

inferred :: fn (value: i32) => value
¬
main :: fn() {
    crlf := "routed\r\n"
    cr := "carriage\rreturn"
    mixed := "first\nsecond\r\n"
    assert crlf.count == 8
    assert cr.count == 15
    assert mixed.count == 14
}
¬
main :: fn () {
    crlf  := "routed\r\n"
    cr    := "carriage\rreturn"
    mixed := "first\nsecond\r\n"

    assert crlf.count == 8
    assert cr.count == 15
    assert mixed.count == 14
}
