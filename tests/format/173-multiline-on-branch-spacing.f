main::fn(event:Event){
on event{
None=>{
}
Closed=>{
close()
}
Resized{width:_}=>{
}
}
}
¬
main :: fn (event: Event) {
    on event {
        None => {
        }

        Closed => {
            close()
        }

        Resized { width: _ } => {
        }
    }
}
