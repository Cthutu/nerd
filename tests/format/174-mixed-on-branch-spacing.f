main::fn(event:Event){on event{None=>{} Escape=>close() Enter=>{toggle_fullscreen()} Other=>ignore() Unknown=>ignore() Closed=>{shutdown()}}}
¬
main :: fn (event: Event) {
    on event {
        None => {
        }

        Escape => close()

        Enter => {
            toggle_fullscreen()
        }

        Other    => ignore()
        Unknown  => ignore()

        Closed => {
            shutdown()
        }
    }
}
