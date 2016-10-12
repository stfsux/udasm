
#ifndef __MCU_DASM_H__
#define __MCU_DASM_H__

extern char* g_udasm_appname;

#define UDASM_USAGE "udasm [options] file\n" \
 "Options:\n" \
 "  -h,--help                          Print this shit.\n" \
 "  -l,--list-mcu                      List all supported architecture(s).\n" \
 "  -v,--version                       Print another shit.\n" \
 "  -a SYNTAX,--asm=SYNTAX             Select an asm syntax.\n" \
 "  -f FORMAT,--format=FORMAT          Input file format (srec, ihex, bin)\n" \
 "  -m ARCH,--mcu=ARCH                 Select an architecture.\n" \
 "  -e cfg,--memory-map=CFG            Use a memory map configuration.\n" \
 "  -s ADDRESS,--start-address=ADDRESS Start address of the input.\n" \
 "                                     Works only for binary file.\n" \
 "\n" \
 "Report bugs to <stfsux@tuxfamily.org>\n"

#define UDASM_VERSION "udasm v0.0.001"

#define err(...) fprintf (stderr, "%s: ", g_udasm_appname);\
                   fprintf (stderr, __VA_ARGS__)
#define inf(...) fprintf (stdout, __VA_ARGS__)

#endif

