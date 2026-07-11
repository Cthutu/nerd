" Vim syntax file
" Language: Nerd

if exists("b:current_syntax")
  finish
endif

syn keyword nerdKeyword fn return break again for on else defer assert as plex union enum ffi mod use pub impl trait where with in test yes no nil undefined
syn keyword nerdType i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 bool isize usize string void box atomic
syn match nerdComment "--.*$"
syn match nerdNumber "\v<\d+(\.\d+)?>"
syn match nerdOperator "::\|:=\|=>\|->\|\.\.<\|\.\.=\|==\|!=\|<=\|>=\|&&\|[|][|]\|[+*/%<>=:|&^$?\\-]"
syn region nerdTripleString start=+"""+ skip=+\\\\\|\\"+ end=+"""+
syn region nerdCString start=+c"+ skip=+\\\\\|\\"+ end=+"+
syn region nerdString start=+"+ skip=+\\\\\|\\"+ end=+"+

hi def link nerdKeyword Keyword
hi def link nerdType Type
hi def link nerdComment Comment
hi def link nerdNumber Number
hi def link nerdOperator Operator
hi def link nerdTripleString String
hi def link nerdCString String
hi def link nerdString String

let b:current_syntax = "nerd"
