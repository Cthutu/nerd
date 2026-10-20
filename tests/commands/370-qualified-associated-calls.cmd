widgets :: use test.public_method
other_widgets :: use test.public_method
factories :: use test.associated_parts
first_module :: use test.associated_a
second_module :: use test.associated_b

main :: fn () -> i32 {
    widget := widgets.Widget.init(42)
    assert widget.get() == 42

    another: widgets.Widget = other_widgets.Widget.init(43)
    assert another.get() == 43

    factory := factories.PartFactory.init()
    assert factory.value == 1
    factory.apply()
    assert factory.value == 2

    first := on first_module.A.new() {
        value => value
        else => return 1
    }
    assert first.id == 42

    second := on second_module.B.new(10, 20) {
        value => value
        else => return 2
    }
    assert second.id == 0

    return 0
}
¬
0
¬
