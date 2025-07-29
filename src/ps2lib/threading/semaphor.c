#include "ps2ctx/threading/semaphor.h"
#include "ps2ctx/ps2_context.h"
#include "ps2ctx/threading/sema_param.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int Kernel_CreateSema(SemaParam* param) {
    int sem_id = -1;
    for (int i = 0; i < ee_semaphore_count; i++) {
        if (!ctx->ee_semaphors[i].is_valid) {
            sem_id = i;
            break;
        }
    }

    if (sem_id == -1) {
        return -1;
    }

    ee_semaphor_t* sema = &ctx->ee_semaphors[sem_id];
    sema->count = param->init_count;
    sema->max_count = param->max_count;
    sema->option = param->option;
    sema->wait_threads = 0;
    sema->is_valid = true;
    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    sema->mutex = _mutex;

    return sem_id;
}

int Kernel_DeleteSema(int id) {
    if (!ctx->ee_semaphors[id].is_valid) {
        return -1;
    }

    ctx->ee_semaphors[id] = (ee_semaphor_t) {};
    return 0;
}

int Kernel_SignalSema(int id) {
    if (ctx->ee_semaphors[id].wait_threads <= 0)
        ctx->ee_semaphors[id].count++;

    return Kernel_iSignalSema(id);
}

int Kernel_iSignalSema(int id) {
    if (ctx->ee_semaphors[id].count == 0) {
        pthread_mutex_unlock(&ctx->ee_semaphors[id].mutex);
        return -2;
    }
    return 0;
} 

int Kernel_WaitSema(int id) {
    ctx->ee_semaphors[id].wait_threads++;

    if (ctx->ee_semaphors[id].count > 0) {
        ctx->ee_semaphors[id].count--;
    }
    pthread_mutex_lock(&ctx->ee_semaphors[id].mutex);
    return  0;
}

int Kernel_PollSema(int id) {
    return Kernel_iPollSema(id);
}

int Kernel_iPollSema(int id) {
    if (ctx->ee_semaphors[id].count > 0)
        ctx->ee_semaphors[id].count--;

    return 0;
}

int Kernel_ReferSemaStatus(int id, SemaParam* param) {
    return Kernel_iReferSemaStatus(id, param);
}

int Kernel_iReferSemaStatus(int id, SemaParam* param) {
    if (!ctx->ee_semaphors[id].is_valid) {
        return -1;
    }

    param->count = ctx->ee_semaphors[id].count;
    param->wait_threads = ctx->ee_semaphors[id].wait_threads;
    param->option = ctx->ee_semaphors[id].option;
    param->max_count = ctx->ee_semaphors[id].max_count;
    return 0;
}
