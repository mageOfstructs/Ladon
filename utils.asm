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

; subroutine to check if A20 line is enabled
; returns: 1 in ax if it is, otherwise 0
ck_a20:
  push ds
  push es
  push bx
  push cx

  mov cx, 0

  xor ax, ax
  mov ds, ax
  not ax
  mov es, ax

  mov bx, 0x7dfe
  mov ax, [ds:bx]
  push ax
  mov byte [ds:bx], 0x00

  add bx, 0x10
  mov ax, [es:bx]
  push ax
  mov byte [es:bx], 0xFF

  cmp byte [ds:bx], 0xFF
  je ck_a20_off

  mov cx, 1

ck_a20_off:
  pop ax
  mov [es:bx], ax
  sub bx, 0x10
  pop ax
  mov [ds:bx], ax

  mov ax, cx

  pop cx
  pop bx
  pop es
  pop ds
  ret

