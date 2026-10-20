Colour::enum{Red, Green, Blue,}

score::fn(colour:Colour)->i32{
return on colour{
Red=>10
Green=>20
Blue=>30
}
}

main::fn(){
Red:Colour=Green
picked:Colour=Red
explicit:Colour=Colour.Red
score(picked)+score(explicit)
}
¬
Colour :: enum {
    Red
    Green
    Blue
}

score :: fn (colour: Colour) -> i32 {
    return on colour {
        Red    => 10
        Green  => 20
        Blue   => 30
    }
}

main :: fn () {
    Red      : Colour = Green
    picked   : Colour = Red
    explicit : Colour = Colour.Red

    score(picked) + score(explicit)
}
¬
Maybe::enum{None Some(i32) Pair(i32,i32) Text(string)}

score::fn(value:Maybe)->i32{
return on value{
None=>0
Maybe.Some(x)=>x
Pair(left,right)=>left+right
Text(_)=>100
}
}
¬
Maybe :: enum {
    None
    Some(i32)
    Pair(i32, i32)
    Text(string)
}

score :: fn (value: Maybe) -> i32 {
    return on value {
        None               => 0
        Maybe.Some(x)      => x
        Pair(left, right)  => left + right
        Text(_)            => 100
    }
}
¬
State::enum {
    Idle -- no work pending
    Running(i32) -- active job identifier that is deliberately long enough to wrap onto another line while keeping the marker aligned
    Failed(string, i32) -- failure payload
    -- standalone after trailing
    Done = 4 -- explicit discriminant
}

Value::union {
    i i32 -- integer value
    text string -- text value
}
¬
State :: enum {
    Idle                 -- no work pending
    Running(i32)         -- active job identifier that is deliberately long
                         --     enough to wrap onto another line while keeping
                         --     the marker aligned
    Failed(string, i32)  -- failure payload
    -- standalone after trailing
    Done = 4  -- explicit discriminant
}

Value :: union {
    i    i32     -- integer value
    text string  -- text value
}
¬
State::enum {
    Idle


    Running(i32) -- active job identifier that is deliberately long enough to wrap onto another line while keeping the marker aligned

    Failed(string)
    -- keep this attached to Done without adding a gap
    Done
}
¬
State :: enum {
    Idle

    Running(i32)  -- active job identifier that is deliberately long enough to
                  --     wrap onto another line while keeping the marker aligned

    Failed(string)
    -- keep this attached to Done without adding a gap
    Done
}
¬
Token::enum {
    EOF=0 -- end marker
    Whitespace
    Identifier=1 -- symbol text
    Number(i32)=10 -- numeric payload

    Text=100 -- string-like token
    Comment
    DoneLonger=101 -- finished
}
¬
Token :: enum {
    EOF         = 0   -- end marker
    Whitespace
    Identifier  = 1   -- symbol text
    Number(i32) = 10  -- numeric payload

    Text       = 100  -- string-like token
    Comment
    DoneLonger = 101  -- finished
}
¬
Event :: enum {
Closed
Resized {
width u16
height u16
}
}
¬
Event :: enum {
    Closed
    Resized {
        width  u16
        height u16
    }
}
