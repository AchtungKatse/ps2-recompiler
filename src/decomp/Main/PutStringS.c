#include "ps2ctx/ps2_context.h"
#include "ps2rc/defines.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void PutStringS(long something, char* format, ...) {
    char textBuffer[0x1000];
	/**/
	/*   va_list args;*/
	/*   va_start(args, format);*/
	/*vsprintf(textBuffer, format, args);*/
	/*   va_end(args);*/

	/*printf(format, ctx->a2.S32[0]);*/
    printf("%s", format);
}

