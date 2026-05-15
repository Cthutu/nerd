" Vim indent file
" Language: Nerd

if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal autoindent
setlocal expandtab
setlocal indentexpr=GetNerdIndent(v:lnum)
setlocal indentkeys=0{,0},0],0),!^F,o,O
setlocal shiftwidth=4
setlocal softtabstop=4

let b:undo_indent = "setlocal autoindent< expandtab< indentexpr< indentkeys< shiftwidth< softtabstop<"

function! s:StripLineComment(line) abort
  let l:in_string = v:false
  let l:escaped = v:false
  let l:i = 0

  while l:i < strlen(a:line)
    let l:ch = strpart(a:line, l:i, 1)

    if l:in_string
      if l:escaped
        let l:escaped = v:false
      elseif l:ch ==# '\'
        let l:escaped = v:true
      elseif l:ch ==# '"'
        let l:in_string = v:false
      endif
      let l:i += 1
      continue
    endif

    if l:ch ==# '"'
      let l:in_string = v:true
      let l:i += 1
      continue
    endif

    if l:ch ==# '-' && l:i + 1 < strlen(a:line) && strpart(a:line, l:i + 1, 1) ==# '-'
      return strpart(a:line, 0, l:i)
    endif

    let l:i += 1
  endwhile

  return a:line
endfunction

function! s:TrimCode(line) abort
  return substitute(s:StripLineComment(a:line), '\s\+$', '', '')
endfunction

function! s:HasUnclosedOpenDelimiter(line) abort
  let l:line = s:StripLineComment(a:line)
  let l:stack = []
  let l:in_string = v:false
  let l:escaped = v:false
  let l:i = 0

  while l:i < strlen(l:line)
    let l:ch = strpart(l:line, l:i, 1)

    if l:in_string
      if l:escaped
        let l:escaped = v:false
      elseif l:ch ==# '\'
        let l:escaped = v:true
      elseif l:ch ==# '"'
        let l:in_string = v:false
      endif
      let l:i += 1
      continue
    endif

    if l:ch ==# '"'
      let l:in_string = v:true
    elseif l:ch =~# '[{[(]'
      call add(l:stack, l:ch)
    elseif l:ch =~# '[}\])]' && !empty(l:stack)
      call remove(l:stack, -1)
    endif

    let l:i += 1
  endwhile

  return !empty(l:stack)
endfunction

function! GetNerdIndent(lnum) abort
  let l:prevnum = prevnonblank(a:lnum - 1)
  if l:prevnum == 0
    return 0
  endif

  let l:indent = indent(l:prevnum)
  let l:prev = s:TrimCode(getline(l:prevnum))
  let l:line = s:TrimCode(getline(a:lnum))

  if s:HasUnclosedOpenDelimiter(l:prev)
    let l:indent += shiftwidth()
  endif

  if l:line =~# '^\s*[}\])]'
    let l:indent -= shiftwidth()
  endif

  return max([l:indent, 0])
endfunction
