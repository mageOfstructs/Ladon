[bits 16]
[org 0x7c00]

mov bx, test
call prt
cli
; disable nmi (bc intel stoopid)
mov dx, 0x70
in ax, dx
or ax, 0x80
out dx, ax

call ck_a20
cmp ax, 1
je a20_done

mov ax, 0x2401 ; an attempt at enabling a20
int 0x15       ; wiki says to straight up ignore anything this returns and just check manually again
call ck_a20
cmp ax, 1
je a20_done

print a20_fail
jmp loop

a20_done:
loop:
  jmp loop

%include "utils.asm"
test: db "Hello World!", 0

times 510-($-$$) db 0
dw 0xAA55
