main::fn(){
first:=open() else [error] {
report(error)
return
}
second:=try_open() else [error]{
report(error)
}
third:=optional() else {}
fourth:=optional() else {again}
fifth:=a_very_long_function_name(first_argument, second_argument, third_argument, fourth_argument) else [error] {
report(error)
}
sixth:=first_operation() else { return second_operation() else {return} }
}
¬
main :: fn () {
    first := open() else [error] {
        report(error)
        return
    }
    second := try_open() else [error] {
        report(error)
    }
    third := optional() else {
    }
    fourth := optional() else {
        again
    }
    fifth := a_very_long_function_name(first_argument,
                                       second_argument,
                                       third_argument,
                                       fourth_argument) else [error] {
        report(error)
    }
    sixth := first_operation() else {
        return second_operation() else {
            return
        }
    }
}
