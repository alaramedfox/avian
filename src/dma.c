/*
GazOS Operating System
Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <dma.h>
#include <asmfunc.h>

/* Defines for accessing the upper and lower byte of an integer. */
#define LOW_BYTE(x)         (x & 0x00FF)
#define HI_BYTE(x)          ((x & 0xFF00) >> 8)

/* Quick-access registers and ports for each DMA channel. */
static const byte MaskReg[8]   = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
static const byte ModeReg[8]   = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
static const byte ClearReg[8]  = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

static const byte PagePort[8]  = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
static const byte AddrPort[8]  = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
static const byte CountPort[8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };

static void __dma_xfer(byte channel, byte page, uint32_t offset, uint32_t length, byte mode);

void dma_xfer(byte channel, addr_t address, size_t length, bool read)
{
	byte page=0, mode=0;
	uint32_t offset = 0;
	
	if(read)
		mode = 0x48 + channel;
	else
		mode = 0x44 + channel;
		
	page = address >> 16;
	offset = address & 0xFFFF;
	length--;
	
	__dma_xfer(channel, page, offset, length, mode);	
}	
		

static void __dma_xfer(byte channel, byte page, uint32_t offset, uint32_t length, byte mode)
{
    /* Don't let anyone else mess up what we're doing. */
    __asm__ ("cli");

    /* Set up the DMA channel so we can use it.  This tells the DMA */
    /* that we're going to be using this channel.  (It's masked) */
    outportb(MaskReg[channel], 0x04 | channel);

    /* Clear any data transfers that are currently executing. */
    outportb(ClearReg[channel], 0x00);

    /* Send the specified mode to the DMA. */
    outportb(ModeReg[channel], mode);

    /* Send the offset address.  The first byte is the low base offset, the */
    /* second byte is the high offset. */
    outportb(AddrPort[channel], LOW_BYTE(offset));
    outportb(AddrPort[channel], HI_BYTE(offset));

    /* Send the physical page that the data lies on. */
    outportb(PagePort[channel], page);

    /* Send the length of the data.  Again, low byte first. */
    outportb(CountPort[channel], LOW_BYTE(length));
    outportb(CountPort[channel], HI_BYTE(length));

    /* Ok, we're done.  Enable the DMA channel (clear the mask). */
    outportb(MaskReg[channel], channel);

    /* Re-enable interrupts before we leave. */
    __asm__ ("sti");
}