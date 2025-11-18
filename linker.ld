OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)

SECTIONS {
	. = 0x8200;

	__ld_kernel_begin = .;
	
	.text : {
		*(.text)
	}
	
	.rodata : {
		*(.rodata)
	}
	
	.data : {
		*(.data)
	}
	
	.bss : {
		*(.bss)
	}

	__ld_kernel_end = .;

	/DISCARD/ : {
		*(.comment)
	}
}