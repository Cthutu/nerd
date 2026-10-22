use std.crypt
use std.io

main :: fn () {
    input: [1000]u8
    input[..] = 'a'
    mark := temp_arena.mark()
    dirty := temp_arena.alloc_bytes(2048)
    dirty[..] = 0xA5
    temp_arena.restore(mark)
    assert sha1(input[0 .. 0].as(string)).show() == "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709"
    assert sha1(input[0 .. 1].as(string)).show() == "86F7E437FAA5A7FCE15D1DDCB9EAEAEA377667B8"
    assert sha1(input[0 .. 3].as(string)).show() == "7E240DE74FB1ED08FA08D38063F6A6A91462A815"
    assert sha1(input[0 .. 43].as(string)).show() == "7BD258F2F4CC4B02FCA4EA157F55F6D88D26D954"
    assert sha1(input[0 .. 55].as(string)).show() == "C1C8BBDC22796E28C0E15163D20899B65621D65A"
    assert sha1(input[0 .. 56].as(string)).show() == "C2DB330F6083854C99D4B5BFB6E8F29F201BE699"
    assert sha1(input[0 .. 57].as(string)).show() == "F08F24908D682555111BE7FF6F004E78283D989A"
    assert sha1(input[0 .. 63].as(string)).show() == "03F09F5B158A7A8CDAD920BDDC29B81C18A551F5"
    assert sha1(input[0 .. 64].as(string)).show() == "0098BA824B5C16427BD7A1122A5A442A25EC644D"
    assert sha1(input[0 .. 65].as(string)).show() == "11655326C708D70319BE2610E8A57D9A5B959D3B"
    assert sha1(input[0 .. 119].as(string)).show() == "EE971065AAA017E0632A8CA6C77BB3BF8B1DFC56"
    assert sha1(input[0 .. 120].as(string)).show() == "F34C1488385346A55709BA056DDD08280DD4C6D6"
    assert sha1(input[0 .. 128].as(string)).show() == "AD5B3FDBCB526778C2839D2F151EA753995E26A0"
    assert sha1(input[0 .. 1000].as(string)).show() == "291E9A6C66994949B57BA5E650361E98FC36B1BA"
    assert sha1("abc").show() == "A9993E364706816ABA3E25717850C26C9CD0D89D"
    binary: [5]u8 = [0, 255, 128, 0, 1]
    assert sha1(binary.as(string)).show() == "DE7AA9053951A0E9BA7B86B79F3039BAF4715DD6"
    before := temp_arena.mark()
    digest := sha1("The quick brown fox jumps over the lazy dog")
    assert temp_arena.mark() == before
    text := digest.show()
    assert text == "2FD4E1C67A2D28FCED849EE1BB76E7391B93EB12"
    prn(text)
}
¬
0
¬
2FD4E1C67A2D28FCED849EE1BB76E7391B93EB12
¬

¬
