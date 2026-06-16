use std.io

helper :: fn () -> i32 {
    return 0
}

main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    i32_size := i32.size
    literal_size := 128.size
    array_size := nums.size
    slice_size := view.size
    array_bytes := nums.bytes
    slice_bytes := view.bytes
    string_size := text.size
    ptr_size := ptr.size
    nil_size := nil.size
    fn_size := helper.size
    void_size := void.size
    prn($"i32={i32_size} literal={literal_size} array={array_size} slice={slice_size} array_bytes={array_bytes} slice_bytes={slice_bytes} string={string_size} ptr={ptr_size} nil={nil_size} fn={fn_size} void={void_size}")
    return (i32_size + literal_size + array_size + slice_size + array_bytes +
        slice_bytes + string_size + ptr_size + nil_size + fn_size +
        void_size).as(i32)
}
¬
92
¬
i32=4 literal=4 array=12 slice=16 array_bytes=12 slice_bytes=12 string=16 ptr=8 nil=0 fn=8 void=0

¬
delete
¬
