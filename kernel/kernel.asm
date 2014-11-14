; ***************************************
; *	This code is at the 0x8000			*
; *	It loads main C kernel code			*
; *	    Stanislav Podolsky				*
; ***************************************

[BITS 32]
[EXTERN kernel_start]
[GLOBAL main]
[GLOBAL FillTable]

;--------------------------------------------

main:

;################################### Page directory is at 0x28000 ################################

; 0x41000 = 0x28000 (PDE) + 100 PDE entries for 100 potential processes

    mov eax, 0x41000 + 10001b		; table #1 is at 0x41000 	[ Kernel's code ]
    mov [cs:0x28000+0x0], eax		; the first element

    mov eax, 0x41000 + 0x1000 + 10011b	; table #1 is at 0x42000 	[ Kernel's data ]
    mov [cs:0x28000+0x4], eax		; the second element

    mov eax, 0x41000 + 0x2000 + 10011b	; table #1 is at 0x43000 	[ Kernel's stack ]
    mov [cs:0x28000+0x8], eax		; the third element

; Feh. Now we have three base segments for out kernel. Now let's prepare other:

mov ecx, 12
mov eax, 0x41000 + 0x3000 + 10011b

.fill_next:

    mov [cs:0x28000+ecx], eax
    add ecx, 4
    add eax, 0x1000
    cmp ecx, 1000
    jng .fill_next


;#################################### Page table #1 CODE at 0x41000 -> 0x8000 ####################

    mov ebx, 0x41000
    mov edi, 0x8111
    call FillTable

    mov eax, 0x8111			; lets map eight PTE to the first code's
    mov [cs:0x41000+32], eax		; page to prevent eip address corruption

		      ; AVA    G    R    D    A    C    W    U    R    P
    mov edi, 0x408000 + 000b + 0b + 0b + 0b + 0b + 1b + 0b + 0b + 1b + 1b ;
    mov ebx, 0x41000 + 0x1000
    call FillTable

		      ; AVA    G    R    D    A    C    W    U    R    P
    mov edi, 0x808000 + 000b + 0b + 0b + 0b + 0b + 1b + 0b + 0b + 1b + 1b ;
    mov ebx, 0x41000 + 0x2000
    call FillTable

;######################################## Enabling of paging #######################################

    mov eax, 0x28000		; address of kernels PDE
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

;######################################### Setting stack ########################################

mov esp, 0xa00000 		; 3 MB of stack memory (4 MB - max)

;######################################### Jumping to the C Kernel ##############################

    mov ecx, last-main
    mov eax, 0x0
    add eax, ecx
    jmp eax

; This function creates page table entries (1024 entries)
; Parameters:
; 	-ebx - PTE base address
; 	-edi - first page's base address with flags

FillTable:

    mov edx, 0
    mov ecx, 0

.fill:

    cmp ecx, 0x400
    je .exit
    mov [cs:ebx+edx], edi
    add edx, 4
    add edi, 0x1000
    inc ecx
    jmp .fill

.exit:

    ret

last:
