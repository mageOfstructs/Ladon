%include "macros.asm"

STAGE2_SIZE equ 0

[bits 16]
[org 0x7c00]

xor ax, ax ; ax -> 0
mov ss, ax ; set stack to the highest (usable) address
mov sp, 0x7BFF
mov bp, sp

; mov cs, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

push dx ; dl = boot drive

print test

disable_ints:
cli
; disable nmi (bc intel stoopid)
mov dx, 0x70
in ax, dx
or ax, 0x80
out dx, ax

load_stage2:

; get drive geometry
pop dx
push dx ; still need it later and mov didn't work
mov ah, 8 ; get drive geometry (apparently this is bad when you use floopies but whatever)
int 0x13
push dx ; save head count

and cl, 0x3f
push cx ; save sectors per track

xor ax, ax
mov es, ax ; reset es because that's a thing

mov ah, 2
mov al, STAGE2_SIZE ; total sector count
mov ch, 0
mov cl, 2 ; second sector (first one is the bootsector)
mov dx, [bp-2] ; get the saved drive number
mov dh, 0
mov bx, STAGE1a_START

int 0x13
jnc read_success

print read_fail
xor bx, bx
mov bl, ah
call prti
jmp loop

read_success:
  jmp STAGE1a_START

loop:
  jmp loop

%include "utils.asm"

test: db "Ladon bootloader Version 0" ; 0x0d = CR, 0x0a = LF
new_line: db 0x0d, 0x0a, 0

read_fail: db "read failed!", 0

times 510-($-$$) db 0
dw 0xAA55
