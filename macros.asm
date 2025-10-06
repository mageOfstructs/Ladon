%macro print 1
mov bx, %1
call prt
%endmacro

%macro println 1
print %1
mov bx, new_line
call prt
%endmacro

%macro endl 0
print new_line
%endmacro

STAGE1a_START equ 0x7EFF
STAGE2_START equ (STAGE1a_START+512)
