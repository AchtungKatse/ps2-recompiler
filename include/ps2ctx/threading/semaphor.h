#pragma once
#include "ps2ctx/threading/sema_param.h"
#include "ps2rc/defines.h"
#include <pthread.h>

typedef struct ee_semaphor {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int option;
    pthread_mutex_t mutex;
    b8 is_valid;

} ee_semaphor_t;

int Kernel_CreateSema(SemaParam* output); // Returnds ID
int Kernel_WaitSema(int id);
int Kernel_SignalSema(int id);
int Kernel_iSignalSema(int id);
int Kernel_PollSema(int id);
int Kernel_iPollSema(int id);
int Kernel_DeleteSema(int id);
int Kernel_iDeleteSema(int id);
int Kernel_ReferSemaStatus(int id, SemaParam* param);
int Kernel_iReferSemaStatus(int id, SemaParam* param);
