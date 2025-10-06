%include "macros.asm"

[bits 16]
[org STAGE1a_START]

a20_start:
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

lgdt [GDT_desc]

println gdt
; enter protected mode
mov eax, cr0
or al, 1
mov cr0, eax
jmp (GDT_code-GDT_start):prot_main

GDT_start:
  times 2 dd 0  ; null entry
GDT_code:
  dw 0xFFFF     ; lower limit
  dw 0          ; lower base
  db 0          ; lower base 2
  db 0b10011010 ; readable code segment with DPL=0
  db 0b11001111 ; granularity, 32bit segment + upper limit
  db 0          ; higher base
GDT_data:
  dw 0xFFFF     ; lower limit
  dw 0          ; lower base
  db 0          ; lower base 2
  db 0b10010010 ; readable data segment with DPL=0
  db 0b11001111 ; granularity, 32bit segment + upper limit
  db 0          ; higher base
GDT_end:

GDT_desc:
  dw GDT_end - GDT_start
  dd GDT_start

loop:
  hlt
  jmp loop

%include "utils.asm"

a20_fail: db "Could not enable the A20 line!", 0
gdt: db "GDT loaded!", 0
new_line: db 0x0d, 0x0a, 0

[bits 32]
prot_main:
  mov ax, 0x10 ; cs already set by long jump?
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov esp, 0x7FFFF ; continue to use this location as it should be safe

  ; HACK: The build system inserts the stage2 code right after this so no jump is neccessary
  ; call STAGE2_START
  ; hlt
  ; jmp prot_main
  ; STAGE2_START equ STAGE1a_START+$$+1
