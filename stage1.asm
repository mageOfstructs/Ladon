%macro print 1
mov bx, %1
call prt
%endmacro

STAGE2_START equ 0x7EFF

%macro println 1
print %1
mov bx, new_line
call prt
%endmacro

%macro endl 0
print new_line
%endmacro

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

load_stage2:

print hnum_msg

pop dx
push dx ; still need it later and mov didn't work
mov ah, 8 ; get drive geometry (apparently this is bad when you use floopies but whatever)
int 0x13
push dx ; save head count
xor bx, bx
mov bl, dh
call prti

print spt_msg
and cl, 0x3f
push cx ; save sectors per track
xor bx, bx
mov bl, cl
call prti
endl

xor ax, ax
mov es, ax

mov ah, 2
mov al, 1 ; total sector count
mov ch, 0
mov cl, 2 ; second sector (first one is the bootsector)
mov dx, [bp-2] ; get the saved drive number
mov dh, 0
mov bx, STAGE2_START

int 0x13
jnc read_success

print read_fail
xor bx, bx
mov bl, ah
call prti
jmp loop

read_success:
lgdt [GDT_desc]

println gdt

; enter protected mode
mov eax, cr0
or al, 1
mov cr0, eax
jmp (GDT_code-GDT_start):prot_main

loop:
  jmp loop

GDT_start:
  times 2 dd 0  ; null entry
GDT_code:
  dw 0xFFFF     ; lower limit
  dw 0          ; lower base
  db 0          ; lower base 2
  db 0b10011010 ; reabable code segment with DPL=0
  db 0b11001111 ; granularity, 32bit segment + upper limit
  db 0          ; higher base
GDT_data:
  dw 0xFFFF     ; lower limit
  dw 0          ; lower base
  db 0          ; lower base 2
  db 0b10010010 ; reabable code segment with DPL=0
  db 0b11001111 ; granularity, 32bit segment + upper limit
  db 0          ; higher base
GDT_end:

GDT_desc:
  dw GDT_end - GDT_start
  dd GDT_start

%include "utils.asm"

test: db "Ladon bootloader Version 0" ; 0x0d = CR, 0x0a = LF
new_line: db 0x0d, 0x0a, 0

a20_fail: db "Could not enable the A20 line!", 0
read_fail: db "read failed!", 0
gdt: db "GDT loaded!", 0
hnum_msg: db "Number of heads: ", 0
spt_msg: db 0x0d, 0x0a, "Sectors per track: ", 0

[bits 32]
prot_main:
  mov ax, 0x10 ; cs already set by long jump?
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov esp, 0x7FFFF ; continue to use this location as it should be safe

  call STAGE2_START
  hlt
  jmp prot_main

times 510-($-$$) db 0
dw 0xAA55
