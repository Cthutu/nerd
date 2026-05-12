exit_strings::fn(exit:string)->string{return on exit{"N"=>"North"
"S"=>"South"
"E"=>"East"
else=>exit}}
¬
exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N" => "North"
        "S" => "South"
        "E" => "East"
        else => exit
    }
}
