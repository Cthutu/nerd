=== case: optional-result-enum-payload-constructors ===
Command :: enum { Text(string) Pair(i32, i32) Empty }
optional :: fn () -> ?Command { return Command.Text("optional") }
result :: fn () -> Command\string { return Command.Pair(20, 22) }
main :: fn () {
    on optional() {
        command => on command { Text(text) => { assert text == "optional" }
                                else => { assert no } }
        else => { assert no }
    }
    on result() {
        command => on command { Pair(a, b) => { assert a + b == 42 }
                                else => { assert no } }
        error! => { assert no, error }
    }
    direct: ?Command = Command.Text("direct")
    on direct {
        command => on command { Text(text) => { assert text == "direct" }
                                else => { assert no } }
        else => { assert no }
    }
}
¬
0
¬

¬

¬
