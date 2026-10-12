pub net_socket :: fn () => NetSocket { next_id: 1 generation: 1 send_ms: net_wait_infinite receive_ms: net_wait_infinite connect_ms: 1000 reconnect_ms: 25 ... }
short :: fn () => Point { x: 1 y: 2 }
long_constructor_name_that_consumes_most_of_the_available_width :: fn () => Item { value: 42 ... }
from_block :: fn () { return NetSocket { next_id: 1 generation: 1 send_ms: net_wait_infinite receive_ms: net_wait_infinite connect_ms: 1000 reconnect_ms: 25 ... } }
impl Config {
    pub create :: fn () => Self { first_value: default_value second_value: default_value third_value: default_value ... }
}
wrap :: fn () {
    factory := fn () => Config { first_value: default_value second_value: default_value third_value: default_value ... }
    _ := factory
}
¬
pub net_socket :: fn () => NetSocket {
    next_id      : 1
    generation   : 1
    send_ms      : net_wait_infinite
    receive_ms   : net_wait_infinite
    connect_ms   : 1000
    reconnect_ms : 25
    ...
}

short :: fn () => Point { x: 1 y: 2 }

long_constructor_name_that_consumes_most_of_the_available_width :: fn () => Item {
    value: 42
    ...
}

from_block :: fn () => NetSocket {
    next_id      : 1
    generation   : 1
    send_ms      : net_wait_infinite
    receive_ms   : net_wait_infinite
    connect_ms   : 1000
    reconnect_ms : 25
    ...
}

impl Config {

    pub create :: fn () => Self {
        first_value  : default_value
        second_value : default_value
        third_value  : default_value
        ...
    }

}

wrap :: fn () {
    factory := fn () => Config {
        first_value  : default_value
        second_value : default_value
        third_value  : default_value
        ...
    }
    _ := factory
}
