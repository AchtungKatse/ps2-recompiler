#include "ps2ctx/ps2_context.h"
#include <threads.h>

RecompilationContext* ctx;
PS2Memory* mem;
EEReg ZeroRegister;
int EEThreadID = 0;
