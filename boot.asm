[bits 16]
[org 0x7c00]

mov bx, test
call prt
loop:
  jmp loop

; subroutine to print a simple string (NUL-terminated) in real mode
; bx: pointer to string
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

test: db "Hello World!", 0

times 510-($-$$) db 0
dw 0xAA55
