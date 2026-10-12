main::fn(){
first:=on open() else [error] {
report(error)
return
}
second:=on try_open() else? [error]{
report(error)
}
third:=on optional() else? {}
fourth:=on optional() else {again}
fifth:=on a_very_long_function_name(first_argument, second_argument, third_argument, fourth_argument) else? [error] {
report(error)
}
sixth:=on first_operation() else { return on second_operation() else {return} }
}
¬
main :: fn () {
    first := on open() else [error] {
        report(error)
        return
    }
    second := on try_open() else? [error] {
        report(error)
    }
    third := on optional() else? {
    }
    fourth := on optional() else {
        again
    }
    fifth := on a_very_long_function_name(first_argument,
                                          second_argument,
                                          third_argument,
                                          fourth_argument) else? [error] {
        report(error)
    }
    sixth := on first_operation() else {
        return on second_operation() else {
            return
        }
    }
}
