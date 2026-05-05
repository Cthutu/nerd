Token::enum {
    EOF=0 -- end marker
    Identifier=1 -- symbol text
    Number(i32)=10 -- numeric payload

    Text=100 -- string-like token
    Done=101 -- finished
}
¬
Token :: enum {
    EOF         = 0  -- end marker
    Identifier  = 1  -- symbol text
    Number(i32) = 10 -- numeric payload

    Text = 100 -- string-like token
    Done = 101 -- finished
}
