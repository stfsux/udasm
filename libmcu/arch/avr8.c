#include <stdlib.h>
#include <stdint.h>
#include "libmcu_arch.h"

char* avr8_mnemonics[] = 
{
  "add", "adc", "adiw", "sub", "subi", "sbc", "sbci",
  "sbiw", "and", "andi", "or", "ori", "eor", "com",
  "neg", "sbr", "cbr", "inc", "dec", "tst", "clr", 
  "ser", "mul", "muls", "mulsu", "fmul", "fmuls", 
  "fmulsu", "des", "rjmp", "ijmp", "eijmp", "jmp",
  "rcall", "icall", "eicall", "call", "ret", "reti",
  "cpse", "cp", "cpc", "cpi", "sbrc", "sbrs", "sbic",
  "sbis", "brbs", "brbc", "breq", "brne", "brcs", "brcc",
  "brsh", "brlo", "brmi", "brpl", "brge", "brlt", "brhs",
  "brhc", "brts", "brtc", "brvs", "brvc", "brie", "brid",
  "mov", "movw", "ldi", "lds", "ldd", "sts", "st", "std",
  "lpm", "elpm", "spm", "in", "out", "push", "pop",
  "xch", "las", "lac", "lat", "lsl", "lsr", "rol",
  "ror", "asr", "swap", "bset", "bclr", "sbi",
  "cbi", "bst", "bld", "sec", "clc", "sen", "cln",
  "sez", "clz", "sei", "cli", "ses", "cls", "sev",
  "clv", "set", "clt", "seh", "clh", "break", "nop",
  "sleep", "wdr"
};

