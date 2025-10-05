%macro print 1
mov bx, %1
call prt
%endmacro

[bits 16]
[org 0x7c00]

push dx ; dl = boot drive

mov ax, 0x7000
mov ss, ax ; set stack to the highest (usable) address
mov sp, 0xFFFF
mov bp, sp

xor ax, ax ; ax -> 0
mov cs, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

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
pop dx
mov ah, 8 ; get drive geometry
int 0x13

mov ah, 2

int 0x13

lgdt [GDT_desc]

print gdt

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
test: db "Ladon bootloader Version 0", 0x0d, 0x0a, 0 ; 0x0d = CR, 0x0a = LF
a20_fail: db "Could not enable the A20 line!", 0
gdt: db "GDT loaded!", 0x0d, 0x0a, 0

[bits 32]
prot_main:
  mov ax, 0x08
  mov cs, ax
  mov ax, 0x10
  mov cs, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov esp, 0x7FFFF ; continue to use this location as it should be safe

  mov byte [0xb8000], 'W'
  hlt
  jmp prot_main

times 510-($-$$) db 0
dw 0xAA55
