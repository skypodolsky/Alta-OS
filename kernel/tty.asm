[BITS 32]

[EXTERN g_tty_x]
[EXTERN g_tty_y]
[GLOBAL arch_tty_byte_output]
[GLOBAL arch_tty_get_mem]
[GLOBAL arch_tty_set_mem]

arch_tty_byte_output:
	push ebp
	mov ebp,esp

	push eax
	push ebx

	xor eax, eax		; we calculating offset according to g_tty_x & g_tty_y
	xor ebx, ebx
	mov al, 80
	mul byte [g_tty_y]
	add ax, [g_tty_x]	; now 'ax' contains the result

	mov bl, [esp + 16]
	mov [0xb0000 + eax * 2], bl

	mov bl, [esp + 20]
	mov [0xb0000 + eax * 2 + 1], bl

	pop ebx
	pop eax

	mov esp,ebp
	pop ebp
	ret

arch_tty_get_mem:
	push ebp
	mov ebp,esp

	push eax
	push ebx
	push ecx			; for address of result

	xor eax, eax		; we calculating offset according to x & y
	xor ebx, ebx
	mov al, 80
	mul byte [esp + 24]
	add ax, [esp + 20]	; now 'ax' contains the result

	mov bx, [0xb0000 + eax * 2]
	mov ecx, [esp + 28]
	mov [ecx], word bx

	pop ecx
	pop ebx
	pop eax

	mov esp,ebp
	pop ebp
	ret

arch_tty_set_mem:
	push ebp
	mov ebp,esp

	push eax
	push ebx
	push ecx			; for address of result

	xor eax, eax		; we calculating offset according to x & y
	xor ebx, ebx
	mov al, 80
	mul byte [esp + 24]
	add ax, [esp + 20]	; now 'ax' contains the result

	mov bx, [esp + 28]
	mov [0xb0000 + eax * 2], bx

	pop ecx
	pop ebx
	pop eax

	mov esp,ebp
	pop ebp

	ret
