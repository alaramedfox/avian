#define VGA_C_SOURCE
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//		File:		      /avian/vga.c
//		Purpose:	      VGA Output driver
// ======================================================================== */
 
#include <vga.h>
#include <asmfunc.h>
 
static vga_t * const vga = (vga_t*)0xb8000;

void move_cursor(byte row, byte col)
{
	word position = (row*VGA_COL) + col;

	// cursor LOW port to VGA INDEX register
	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (byte)(position&0xFF));
	// cursor HIGH port to VGA INDEX register
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (byte)((position>>8)&0xFF));
}

void hide_cursor(void)
{
	move_cursor(25,0);
}

void 	 vga_increment(void)		{ vga->vptr++; }
void 	 vga_decrement(void)		{ vga->vptr--; }
size_t vga_getcol(void) 		{ return vga->vptr % VGA_COL; }
size_t vga_getrow(void) 		{ return vga->vptr / VGA_COL; }
size_t vga_getloc(void) 		{ return vga->vptr; }
void	 vga_setcolor(color_t c){ vga->color = c; }
void 	 vga_newline(void)		{ vga_movexy(vga_getrow()+1,0); }
void 	 vga_creturn(void) 		{ vga_movexy(vga_getrow(), 0); }

void vga_tabchar(void) 
{
	int spaces = vga_getcol() % 5;
	if(spaces == 0) spaces = 5;
	
	foreach(i, spaces) {
		vga_write(' ');
		vga_increment();
	}
}

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

char vga_char_at(byte row, byte col)
{
	return vga->buffer[(row*VGA_COL+col)*2];
}

void vga_clear(void)
{
	vga->vptr = 0;
	foreach(i, VGA_LEN) {
		vga_write(' ');
		vga_color(vga->color);
		vga_increment();
	}
	vga->vptr = 0;
}

void vga_clear_row(byte row)
{
	vga_movexy(row, 0);
	foreach(i, VGA_COL) {
		addch(' ');
	}
	vga_movexy(row, 0);
}

/* Safely write a single character to vram without segfault */
void vga_write(char value) 
{
	vga->buffer[vga->vptr*2] = value;
	vga_color(vga->color);
}

void vga_color(color_t value) 
{
	vga->buffer[vga->vptr*2+1] = value; 
}

void addch(const char c)
{
	vga_write(c);
	vga_increment();
}
 
void print(const char str[])
{
	for(size_t i=0; str[i] != '\0'; i++) {
		switch(str[i])
		{
			case '\n': vga_newline(); break;
			case '\r': vga_creturn(); break;
			case '\t': vga_tabchar(); break;
			
			default: addch(str[i]); break;
		}
	}
}

void println(const char str[])
{
   print(str); print("\n");
}

void vga_scroll(void) 
{
	/* Shift every row up one row */
	for(size_t i=0; i<VGA_BYTES-(VGA_COL*2); i++) {
		vga->buffer[i] = vga->buffer[i+VGA_COL*2];
	}

	/* Draw empty row at the bottom */
	vga_movexy(24,0);
	foreach(i, VGA_COL) {
		vga_write(' ');
		vga_color(0x07);
		vga->vptr++;
	}
	
	vga_movexy(24,0);
}