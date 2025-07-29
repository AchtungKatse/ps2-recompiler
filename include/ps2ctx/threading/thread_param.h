#pragma once

// Thread statuses
typedef enum ps2_thread_state {
    THS_RUN = 1,
    THS_READY = 2,
    THS_WAIT = 4,
    THS_SUSPEND = 0x8,
    THS_WAITSUSPEND = 0xC,
    THS_DORMANT = 0x10,
} ps2_thread_state_t;

typedef struct thread_param {
    ps2_thread_state_t status;
    int function;
    int stack;
    // void *function;
    // void *stack;
    int stackSize;
    // void *gpRegister;
    int gpRegister;
    int initialPriority;
    int currentPriority;
    unsigned int attr;
    unsigned int option;
} ps2_thread_param_t;

