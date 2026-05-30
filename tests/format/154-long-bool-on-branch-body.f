main::fn(){return on keysym{A=>ScanCode.A VeryLongPattern=>ScanCode.B else=>on (keysym>='0'&&keysym<='9')=>keysym.as(ScanCode) else on (keysym>='A'&&keysym<='Z')=>keysym.as(ScanCode) else on (keysym>='a'&&keysym<='z')=>(keysym-32).as(ScanCode) else ScanCode.Unknown}}
¬
main :: fn () {
    return on keysym {
        A                => ScanCode.A
        VeryLongPattern  => ScanCode.B
        else => on (keysym >= '0' && keysym <= '9') => keysym.as(ScanCode)
            else on (keysym >= 'A' && keysym <= 'Z') => keysym.as(ScanCode)
            else on (keysym >= 'a' && keysym <= 'z') => (keysym - 32).as(ScanCode)
            else ScanCode.Unknown
    }
}
