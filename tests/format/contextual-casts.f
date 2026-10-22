main :: fn(){
a:u32=42
b:u64=a.as( )
values:=[1,2,3]
view:[]i32=values.as()
prefix:[]i32=values.data.as(1+1)
explicit:[]i32=values.data.as([]i32,2)
}
¬
main :: fn () {
    a        : u32   = 42
    b        : u64   = a.as()
    values           := [1, 2, 3]
    view     : []i32 = values.as()
    prefix   : []i32 = values.data.as(1 + 1)
    explicit : []i32 = values.data.as([]i32, 2)
}
