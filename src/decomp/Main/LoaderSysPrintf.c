#include "ps2ctx/ps2_context.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int LoaderSysPrintf(char* format, ...) {
    char textBuffer[256];

    format = FindRam((u64)format);

	/*   va_list args;*/
	/*   va_start(args, format);*/
	/*vsprintf(textBuffer, format, args);*/
	/*   va_end(args);*/


	/*   printf(format);*/
	/*return strlen(format);*/
    return 0;
}
