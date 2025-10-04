; subroutine to print a simple string (NUL-terminated) in real mode
; bx: pointer to string
; returns: nothing
prt:
  push ax
  mov ah, 0x0e

prt_loop:
  mov al, [bx]
  cmp al, 0
  je prt_end
  int 0x10
  inc bx
  jmp prt_loop

prt_end:
  pop ax
  ret

