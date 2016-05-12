#ifndef LCD_H
#define	LCD_H

#include "ILI9163C.h"       //LCD functions
void draw_character(unsigned char c, unsigned char x, unsigned char y);
void draw_string(unsigned char *m, unsigned char x, unsigned char y);

#endif	/* LCD_H */

