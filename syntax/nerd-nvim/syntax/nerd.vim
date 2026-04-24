" Vim syntax file
" Language: Nerd

if exists("b:current_syntax")
  finish
endif

syn keyword nerdKeyword fn return break continue for on else true false
syn keyword nerdType i32 u32 f64 bool string void
syn match nerdComment "--.*$"
syn match nerdNumber "\v<\d+(\.\d+)?>"
syn match nerdOperator "::\|:=\|=>\|->\|\.\.<\|\.\.=\|==\|!=\|<=\|>=\|&&\|[|][|]\|[+*/%<>=:|&^@$-]"
syn region nerdString start=+"+ skip=+\\\\\|\\"+ end=+"+

hi def link nerdKeyword Keyword
hi def link nerdType Type
hi def link nerdComment Comment
hi def link nerdNumber Number
hi def link nerdOperator Operator
hi def link nerdString String

let b:current_syntax = "nerd"
