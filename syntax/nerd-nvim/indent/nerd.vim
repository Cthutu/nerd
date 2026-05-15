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

function! s:ScanDelimiterLine(line, lnum, stack, last_closed) abort
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
      call add(a:stack, [l:ch, a:lnum, l:i])
    elseif l:ch =~# '[}\])]' && !empty(a:stack)
      let a:last_closed[0] = remove(a:stack, -1)
    endif

    let l:i += 1
  endwhile
endfunction

function! s:MatchingOpenContinuationLine(lnum) abort
  let l:depth = 0
  let l:line = a:lnum

  while l:line >= 1
    let l:text = s:StripLineComment(getline(l:line))
    let l:i = strlen(l:text) - 1

    while l:i >= 0
      let l:ch = strpart(l:text, l:i, 1)

      if l:ch ==# ')' || l:ch ==# ']'
        let l:depth += 1
      elseif l:ch ==# '(' || l:ch ==# '['
        let l:depth -= 1
        if l:depth == 0
          return l:line
        endif
      endif

      let l:i -= 1
    endwhile

    let l:line -= 1
  endwhile

  return 0
endfunction

function! s:UnclosedOpenDelimiter(line) abort
  let l:stack = []
  let l:last_closed = [['', 0, -1]]
  call s:ScanDelimiterLine(a:line, 0, l:stack, l:last_closed)

  if empty(l:stack)
    return ['', -1]
  endif

  return [l:stack[-1][0], l:stack[-1][2]]
endfunction

function! GetNerdIndent(lnum) abort
  let l:prevnum = prevnonblank(a:lnum - 1)
  if l:prevnum == 0
    return 0
  endif

  let l:indent = indent(l:prevnum)
  let l:prev = s:TrimCode(getline(l:prevnum))
  let l:line = s:TrimCode(getline(a:lnum))

  if l:prev =~# '[)\]]'
    let l:open_line = s:MatchingOpenContinuationLine(l:prevnum)
    if l:open_line > 0
      let l:indent = indent(l:open_line)
    endif
  endif

  let l:open = s:UnclosedOpenDelimiter(l:prev)
  if l:open[0] ==# '{'
    let l:indent += shiftwidth()
  elseif l:open[0] ==# '(' || l:open[0] ==# '['
    let l:indent = l:open[1] + 1
  endif

  if l:line =~# '^\s*[}\])]'
    let l:indent -= shiftwidth()
  endif

  return max([l:indent, 0])
endfunction
