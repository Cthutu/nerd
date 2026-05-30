main :: fn () {
    -- Normalise verb
    for i, verb_group in state.verbs {
        for verb_synonym in verb_group^ {
            on verb == verb_synonym^ => verb = state.verbs[i][0]
        }
    }

    on verb {
        "q", "quit" => {
            prn("Goodbye!")
            break
        }
        "n", "s", "e", "w", "u", "d" => move(state, verb)
        else => {
            prn("I don't understand that command.")
        }
    }
}

exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N" => "North"
            "S" => "South"
            "E" => "East"
            "W" => "West"
            "U" => "Up"
            "D" => "Down"
        else => exit
    }
}
¬
main :: fn () {
    -- Normalise verb
    for i, verb_group in state.verbs {
        for verb_synonym in verb_group^ {
            on verb == verb_synonym^ => verb = state.verbs[i][0]
        }
    }

    on verb {
        "q", "quit" => {
            prn("Goodbye!")
            break
        }
        "n", "s", "e", "w", "u", "d" => move(state, verb)
        else => {
            prn("I don't understand that command.")
        }
    }
}

exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N"   => "North"
        "S"   => "South"
        "E"   => "East"
        "W"   => "West"
        "U"   => "Up"
        "D"   => "Down"
        else  => exit
    }
}
