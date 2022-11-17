
.text
.align 4
.org    0x400
.globl start
start:

msr daifset, #0xf
b _patch

