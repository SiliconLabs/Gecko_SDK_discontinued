/* Common linker file used by GNU for ARM CORTEX-M3 - platform
 * This file is used as a generic linker script file for all
 * Cortex-M3 microcontrollers with standard memory layout.
 * The controller should have at least 256kB ROM at Address 0x0000 0000
 * and 10k Byte RAM at address 0x2000 0000.
 * Since these are default settings (and only the size may by different)
 * it should work with most Cortex-M3 mcu's.
 * 2011 Jan 24 
 */
 
SEARCH_DIR( "$(ToolDir)\..\arm-hitex-elf\lib\thumb2" )
/* Path setting depends on the gnu compiler version */
/* Adjust the path here to the used version as necessary */
SEARCH_DIR("$(ToolDir)\..\lib\gcc\arm-hitex-elf\$(GNU_CORTEX_VERSION)\thumb2" )

INPUT (
   /* HiTOP will automatically put in here all object files to be linked.
      Leave this unchanged!                                               */
   $(LinkObjects)
)

/* Target Specific Parameters */

MEMORY
{
	VECTORFLASH (rx)    : ORIGIN = 0x00000000,	LENGTH = (0xE0)
	CRCFLASH (rx)		: ORIGIN = 0x000000E0,	LENGTH = 0x20
	FLASH (rx)          : ORIGIN = 0x00000100,	LENGTH = 0x1FF00
	SRAM  (rwx)         : ORIGIN = 0x20000000,	LENGTH = 0x4000
}

/* Layout Information */
/* Section Definitions */
SECTIONS
{
	.vectors :
	{		
		__vectors_start = .;
		KEEP(*(_vector));
		. = ALIGN(4);
	} > VECTORFLASH
	
	/******************************************/
	.CRCs :
	{
		PROVIDE(_crc_location = .);
		/* *(._CRC_area)*/
		KEEP(*(_CRC_area));
		. = ALIGN(4);
	} > CRCFLASH
	
	/******************************************/
	.reset :
	{	
		*(.IEC60335_B_beforeReset)
		
		__startup_code__ = .;
		KEEP(*(._Reset));
		__startup_code_end__ = .;
		. = ALIGN(4);
		
	} > FLASH
	
	/******************************************/
	.text :
	{
		__text_start = .;
		PROVIDE(_stext = .);

		. = ALIGN(4);
		
		_IEC60335_code_ = .;
		*(.IEC60335_code)
		_IEC60335_code_end_ = .;
		
		*(.IEC60335_B_PCTestCodeA)
		*(.IEC60335_B_PCTestCodeB)
		*(.IEC60335_B_PCTestCodeC)
		*(.IEC60335_B_PCTestCodeD)
		*(.IEC60335_B_PCTestCodeE)
		*(.IEC60335_B_PCTestCodeF)
		*(.text .text.*)
		
		*(.gnu.linkonce.t.*)
		*(.glue_7)
		*(.glue_7t)
		*(.gcc_except_table)
		*(.vfp11_veneer)
		*(.janus_2cc_veneer)
		*(.v4_bx)
		*(.eh_frame)
		*(.rodata .rodata*)	        /* Read only data */
		*(.gnu.linkonce.r.*)
	
		PROVIDE(_etext = .);
	} > FLASH
	PROVIDE(__text_end = .);        /* End of Text*/

	/******************************************/
	/* data section */
	.data : AT (__text_end)
	{
		. = ALIGN(4);
		PROVIDE(_sdata = .);		/* Start .data section */
		__data_start = .;
		*(.data.privileged)         /* Privileged Data memory */
		. = ALIGN(4);
		__privileged_data_end = .;

		*(vtable vtable.*)
		*(.IEC60335_B_beforeRAMcode)
		*(.RAMcode)
		*(.IEC60335_B_behindRAMcode)
		*(.data .data.*) 			/* Data memory */

		*(.gnu.linkonce.d*)
		/* End .data section */
		__data_end = .;
		PROVIDE(_edata = .);

	} > SRAM
	
	/******************************************/
	/* For no-init variables section */
	.bss (NOLOAD):
	{
		. = ALIGN(4);
	
		/* Start .bss section */
		PROVIDE(_sbss = .);
		__bss_start = .;
	
		*(.bss .bss.*)				/* Zero-filled run time allocate data memory */
		*(COMMON)
		*(.gnu.linkonce.b*)
		/* End .bss section */
		__bss_end = .;
		PROVIDE(_ebss = .);
	
	} > SRAM

	/******************************************/
	/* For stack section */
	estack = ORIGIN(SRAM) + LENGTH(SRAM) - 4;

	.stack (NOLOAD):
	{
		/* this is the start address for memory allocation */
/*		PROVIDE( end = .); */
		. = ALIGN(4);

		/* Start .stack section */
		__stack_start = .;

		*(.stack .stack.*)

		/* End .stack section */
		__stack_end = .;
		
		PROVIDE( _initial_sp = estack);

	} > SRAM 

    /* Stabs debugging sections.  */
        .stab          0 : { *(.stab) }
        .stabstr       0 : { *(.stabstr) }
        .stab.excl     0 : { *(.stab.excl) }
        .stab.exclstr  0 : { *(.stab.exclstr) }
        .stab.index    0 : { *(.stab.index) }
        .stab.indexstr 0 : { *(.stab.indexstr) }
        .comment       0 : { *(.comment) }
    /* DWARF debug sections.
    Symbols in the DWARF debugging sections are relative to the beginning
    of the section so we begin them at 0.  */
    /* DWARF 1 */
        .debug          0 : { *(.debug) }
        .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
        .debug_srcinfo  0 : { *(.debug_srcinfo) }
        .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
        .debug_aranges  0 : { *(.debug_aranges) }
        .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
        .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
        .debug_abbrev   0 : { *(.debug_abbrev) }
        .debug_line     0 : { *(.debug_line) }
        .debug_frame    0 : { *(.debug_frame) }
        .debug_str      0 : { *(.debug_str) }
        .debug_loc      0 : { *(.debug_loc) }
        .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
        .debug_weaknames 0 : { *(.debug_weaknames) }
        .debug_funcnames 0 : { *(.debug_funcnames) }
        .debug_typenames 0 : { *(.debug_typenames) }
        .debug_varnames  0 : { *(.debug_varnames) }
}
