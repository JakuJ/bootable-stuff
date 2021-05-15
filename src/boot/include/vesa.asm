; vbe_set_mode:
; Sets a VESA mode
; In\	AX = Width
; In\	BX = Height
; In\	CL = Bits per pixel
; Out\	FLAGS = Carry clear on success
; Out\	Width, height, bpp, physical buffer, all set in vbe_screen structure

vbe_set_mode:
	mov [.width], ax
	mov [.height], bx
	mov [.bpp], cl
 
	sti
 
	push es					      ; some VESA BIOSes destroy ES, or so I read
	mov ax, 0x4F00				; get VBE BIOS info
	mov di, vbe_info_block
	int 0x10
	pop es
 
	cmp ax, 0x4F				  ; BIOS doesn't support VBE?
	jne .error
 
	mov ax, word[vbe_info_block.video_modes]
	mov [.offset], ax
	mov ax, word[vbe_info_block.video_modes+2]
	mov [.segment], ax
 
	mov ax, [.segment]
	mov fs, ax
	mov si, [.offset]
 
.find_mode:
	mov dx, [fs:si]
	add si, 2
	mov [.offset], si
	mov [.mode], dx
	mov ax, 0
	mov fs, ax
 
	cmp dword [.mode], 0xFFFF			; end of list?
	je .error
 
	push es
	mov ax, 0x4F01				; get VBE mode info
	mov cx, [.mode]
	mov di, mode_info_block
	int 0x10
	pop es
 
	cmp ax, 0x4F
	jne .error
 
	mov ax, [.width]
	cmp ax, [mode_info_block.width]
	jne .next_mode
 
	mov ax, [.height]
	cmp ax, [mode_info_block.height]
	jne .next_mode
 
	mov al, [.bpp]
	cmp al, [mode_info_block.bpp]
	jne .next_mode
 
	; If we make it here, we've found the correct mode!
	mov ax, [.width]
	mov word[vbe_screen.width], ax
	mov ax, [.height]
	mov word[vbe_screen.height], ax
	mov eax, [mode_info_block.framebuffer]
	mov dword[vbe_screen.physical_buffer], eax
	mov ax, [mode_info_block.pitch]
	mov word[vbe_screen.bytes_per_line], ax
	mov eax, 0
	mov al, [.bpp]
	mov byte[vbe_screen.bpp], al
	shr eax, 3
	mov dword[vbe_screen.bytes_per_pixel], eax

	mov ax, [.width]
	shr ax, 3
	dec ax
	mov word[vbe_screen.x_cur_max], ax

	mov ax, [.height]
	shr ax, 4
	dec ax
	mov word[vbe_screen.y_cur_max], ax

	; Set the mode
	push es
	mov ax, 0x4F02
	mov bx, [.mode]
	or bx, 0x4000			; enable LFB
	mov di, 0			; not sure if some BIOSes need this... anyway it doesn't hurt
	int 0x10
	pop es

	cmp ax, 0x4F
	jne .error

	clc
	ret

.next_mode:
	mov ax, [.segment]
	mov fs, ax
	mov si, [.offset]
	jmp .find_mode

.error:
	stc
	ret

.width:				  dw 0
.height:				dw 0
.bpp:				    db 0
.segment:			  dw 0
.offset:				dw 0
.mode:				  dw 0

vbe_info_block:
	.signature:		  db "VBE2"	      ; indicate support for VBE 2.0+
	.version:       dw 0            ; VBE version; high byte is major version, low byte is minor version
  .oem:           dd 0   ; segment:offset pointer to OEM
  .capabilities:  dd 0   ; bitfield that describes card capabilities
  .video_modes:   dd 0   ; segment:offset pointer to list of supported video modes
  .video_memory:  dw 0            ; amount of video memory in 64KB blocks
  .software_rev:  dw 0            ; software revision
  .vendor:        dd 0   ; segment:offset to card vendor string
  .product_name:  dd 0   ; segment:offset to card model name
  .product_rev:   dd 0   ; segment:offset pointer to product revision
  .reserved:      times 222 db 0  ; reserved for future expansion
  .oem_data:      times 256 db 0  ; OEM BIOSes store their strings in this area

mode_info_block:
  .attributes:              dw 0
  .window_a:                db 0
  .window_b:                db 0
  .granularity:             dw 0
  .window_size:             dw 0
  .segment_a:               dw 0
  .segment_b:               dw 0
  .win_func_ptr:            dd 0
  .pitch:                   dw 0
  .width:                   dw 0
  .height:                  dw 0
  .w_char:                  db 0
  .y_char:                  db 0
  .planes:                  db 0
  .bpp:                     db 0
  .banks:                   db 0
  .memory_model:            db 0
  .bank_size:               db 0
  .image_pages:             db 0
  .reserved0:               db 0
  .red_mask:                db 0
  .red_position:            db 0
  .green_mask:              db 0
  .green_position:          db 0
  .blue_mask:               db 0
  .blue_position:           db 0
  .reserved_mask:           db 0
  .reserved_position:       db 0
  .direct_color_attributes: db 0
  .framebuffer:             dd 0
  .off_screen_mem_off:      dd 0
  .off_screen_mem_size:     dw 0
  .reserved1:               times 206 db 0


global vbe_screen
vbe_screen:
  .width:				      dw 0
  .height:				    dw 0
  .bpp:               db 0
  .physical_buffer:   dd 0
  .bytes_per_pixel:   dd 0
  .bytes_per_line:    dw 0
  .x_cur_max:         dw 0
  .y_cur_max:         dw 0
