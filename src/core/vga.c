#define VGA_C_SOURCE
#include <vga.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/core/vga.c
 *		Purpose:	Video output handler
 */
 
vga_t * const vga = (vga_t*)0xb8000;

/* void update_cursor(int row, int col)
 * by Dark Fiber
 */
void move_cursor(byte row, byte col)
{
	word position = (row*VGA_COL) + col;

	// cursor LOW port to VGA INDEX register
	ASM_write_port(0x3D4, 0x0F);
	ASM_write_port(0x3D5, (byte)(position&0xFF));
	// cursor HIGH port to VGA INDEX register
	ASM_write_port(0x3D4, 0x0E);
	ASM_write_port(0x3D5, (byte)((position>>8)&0xFF));
}

void hide_cursor(void)
{
	move_cursor(25,0);
}

void vga_increment(void)		{ vga->vptr++; }
void vga_decrement(void)		{ vga->vptr--; }
size_t vga_getcol(void) 		{ return vga->vptr % VGA_COL; }
size_t vga_getrow(void) 		{ return vga->vptr / VGA_COL; }
size_t vga_getloc(void) 		{ return vga->vptr; }
void	vga_setcolor(color_t c) { vga->color = c; }
void vga_newline(void)			{ vga_movexy(vga_getrow()+1,0); }
void vga_creturn(void) 			{ vga_movexy(vga_getrow()+1, vga_getcol()); }

void vga_movexy(byte row, byte col) 
{
	if(row > VGA_ROW || col > VGA_COL) {
		return;
	}
	else {
		vga_moveptr(row * VGA_COL + col);
	}
}

void vga_moveptr(word v) 
{
	if(v > VGA_LEN -1) {
		vga_scroll();
	}
	else {
		vga->vptr = v; 
	}
}

void vga_clear(void)
{
	vga->vptr = 0;
	for(size_t i=0; i<VGA_LEN; i++) {
		vga_write(' ');
		vga_color(0x07);
		vga_increment();
	}
	vga->vptr = 0;
}

/* Safely write a single character to vram without segfault */
void vga_write(char value) 
{
	vga->buffer[vga->vptr*2] = value;
}

void vga_color(color_t value) 
{
	vga->buffer[vga->vptr*2+1] = value; 
}

void vga_scroll(void) 
{
	/* Shift every row up one row */
	for(size_t i=0; i<=VGA_LEN - VGA_COL; i++) {
		vga->buffer[i] = vga->buffer[i+VGA_COL];
	}

	/* Draw empty row at the bottom */
	vga_movexy(23,0);
	for(size_t i=0; i<VGA_COL; i++) {
		vga_write(' ');
		vga_color(0x07);
		vga->vptr++;
	}
	
	vga_movexy(23,0);
}