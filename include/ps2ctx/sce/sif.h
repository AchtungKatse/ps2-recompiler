#pragma once

struct SifDmaTransfer {
    int src;
    int dest;
    int size;
    int attr;
};

unsigned int Kernel_SifSetDma(struct SifDmaTransfer* transfer, int len);
