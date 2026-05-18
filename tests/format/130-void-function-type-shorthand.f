Sink :: fn (value: i32)

App :: plex {
    init fn (app:^App)
    kill fn (app:^App)
    logic fn (app:^App, delta_time: f32)
    render fn (app:^App)
}

main :: fn () {
    _app: App = {
        quit: false
        init: app_init
        kill: nil
        logic: app_logic
        render: app_render
    }
}
¬
Sink :: fn (value: i32)

App :: plex {
    init   fn (app: ^App)
    kill   fn (app: ^App)
    logic  fn (app: ^App, delta_time: f32)
    render fn (app: ^App)
}

main :: fn () {
    _app: App = {
        quit   : no
        init   : app_init
        kill   : nil
        logic  : app_logic
        render : app_render
    }
}
