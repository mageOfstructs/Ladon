[bits 16]
[org 0x7c00]

mov bx, test
call prt
loop:
  jmp loop

%include "utils.asm"
test: db "Hello World!", 0

times 510-($-$$) db 0
dw 0xAA55
