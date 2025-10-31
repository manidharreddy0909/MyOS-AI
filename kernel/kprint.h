#ifndef KPRINT_H
#define KPRINT_H

#include <stdarg.h>
#include <stdint.h>

/* Print a string directly to the screen */
void kprint(const char *str);

/* Print a single character */
void kputc(char c);

/* Print formatted output like printf */
void kprintf(const char *fmt, ...);

/* Clear the screen */
void clear_screen(void);

#endif
