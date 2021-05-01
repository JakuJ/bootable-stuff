section .init
	; GCC will nicely put the contents of crtend.o's .init section here.
	pop ebp
	ret

section .fini
	; GCC will nicely put the contents of crtend.o's .fini section here.
	pop ebp
	ret
