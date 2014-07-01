[BITS 16]
[ORG 0x7c00]

%define MAX_SIZE_KBYTES 20							; maximal kernel size in kbytes
%define MAX_SIZE_BYTES MAX_SIZE_KBYTES * 1024		; maximal kernel size in bytes
%define MAX_SIZE_SECT (MAX_SIZE_BYTES / 512) + 1	; maximal kernel size in sectors

_start:

    cli			; Disabling interrupts..

    mov ah, 0x00 	; Enabling 80x25 text mode
    mov al, 0x03
    int 10h

    mov ah, 1		; Disabling cursor..
    mov ch, 0x20
    int 0x10

    mov ax, cs 		; Setting segment registers
    mov ds, ax
    mov ss, ax
    mov sp, _start

; Getting amount of sectors per one track

	xor cx, cx

.read_params:
    mov ah, 0x08
    mov dl, 0x80
    int 13h

    jc .read_params	; die, but DO that

    and cl, 0x3f	; maximal amount of sectors per one track

; Reading the disk memory

	push word 0		; offset
	push cx			; amount of sectors

; Checking for BIOS INT 13h extensions
.check_ext:

	mov ah, 0x41
	mov bx,	0x55AA
	mov dl, 0x80
	int 0x13

	jc .check_ext		; error: no int 13h extensions

.read_sectors:

	mov si, DAPACK		; address of "disk address packet"
	mov ah, 0x42		; AL is unused
	mov dl, 0x80		; drive number 0 (OR the drive # with 0x80)
	int 0x13
	jc .read_sectors

load_next:

; Now we loaded our kernel. It is time to enter protected mode

lgdt [gd_reg]		; GDT loading

in al, 0x92		; Enabling A20 mode..
or al, 2
out 0x92, al

mov eax, cr0		; Entering protected mode..
or al, 1
mov cr0, eax

jmp 0x8 : _protected

[BITS 32]

_protected:

mov eax, 0
mov fs, eax
mov gs, eax

mov eax, 16
mov ds, eax

mov eax, 24
mov ss, eax
mov es, eax

mov [cs:0xb8000+0], byte '.'

jmp 0x8:0x7e00

; 'hlt' will not execute. it is here just "шоб було".
hlt

; Let's describe global descriptor's table:
; The first one is the 'dummy' segment filled with nulls
; The second one is the code segment
; The third one is the data segment

gdt:

dw 0, 0, 0, 0	; Zero descriptor

db 0xFF		; Code segment -> Base: 0x0, Limit: 4 GB KB, DPL: 0
db 0xFF
db 0x00
db 0x00
db 0x00
db 10011010b 	;
db 11001111b
db 0x00

db 0xFF		; Data segment -> Base: 0x0 MB, Limit: 4 GB KB, DPL: 0
db 0xFF		; limit
db 0x00		; the 4-st base
db 0x00		; the 3-d base
db 0x00		; the 2-nd base
db 10010010b  ;001
db 11001111b
db 0x00		; the 1-st base

db 0xFF		; Stack segment -> Base: 0x0 MB, Limit: 4 GB KB, DPL: 0
db 0xFF		; limit
db 0x00		; the 4-st base
db 0x00		; the 3-d base
db 0x00		; the 2-nd base
db 10010110b  ;011
db 11001111b
db 0x00		; the 1-st base

gd_reg:		; GDTR register value
dw 8291
dd gdt

; For INT 13h LBA reading

DAPACK:
	db	0x10
	db	0

blkcnt:
	dw	MAX_SIZE_SECT		; INT 13h resets this to # of blocks actually read/written

db_add:
	dw	0x8000				; memory buffer destination address (0:7c00)
	dw	0					; in memory page zero

d_lba:
	dd	1					; put the lba to read in this spot
	dd	0					; more storage bytes only for big lba's ( > 4 bytes )

times 510-($-$$) db 0
dw 0xAA55
