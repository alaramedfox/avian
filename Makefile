# ========================================================================== #
#     Avian Kernel Makefile
#     Revision 1
# ========================================================================== #

# Define the compiler
CC = colorgcc

# Target-specific flags
CARCH = -mtune=i386 -m32 -ffreestanding -fno-exceptions -std=c99 -nostdlib

# Warning options
CWARN = -Wall -Wextra -Werror -Wfatal-errors -Wno-unused

