#include "ps2ctx/sce/sif.h"
#include "ps2ctx/ps2_context.h"
#include <string.h>

unsigned int Kernel_SifSetDma(struct SifDmaTransfer* transfer, int len) {
    void* src = FindRam(transfer->src);
    void* dst = FindRam(transfer->dest);
    memcpy(src, dst, transfer->size);
    return 0;
}
