#include "ps2ctx/ps2_context.h"
#include "ps2ctx/threading/threading.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* FunctionLookup(u64 address);

ps2_thread_param_t threads[256];
int currentThreadIndex = 0;


typedef struct {
    void (*function)(void*);
    void* args;
    int threadID;
} _StartThreadData;

int Recomp_CreateThread(ps2_thread_param_t *param) {
    ps2_thread_param_t* thread = &threads[currentThreadIndex];
    *thread = *param;

    thread->status = THS_DORMANT;
    return currentThreadIndex++;
}

void _StartThread(_StartThreadData* data) {
    ctx = malloc(sizeof(RecompilationContext));
    memset(ctx, 0, sizeof(RecompilationContext));

    ctx->gp.U32[0] = (61 - data->threadID) * MB;
    ctx->sp.U32[0] = (62 - data->threadID) * MB;

    EEThreadID = data->threadID,
    data->function(data->args);
}

void Recomp_StartThread(int threadId, void *arg) {
    void (*function)(void*) = FunctionLookup(threads[threadId].function);

    function(FindRam((u64)arg));
    pthread_t thread;
    _StartThreadData data;
    data.function = function;
    data.args = arg;
    data.threadID = threadId;
    pthread_create(&thread, NULL, (void*)&_StartThread, &data);
}

void Recomp_ReferThreadStatus(int threadId, ps2_thread_param_t *param) {
    ps2_thread_param_t* _param = FindRam((u64)param);
    *_param = threads[threadId];
    ctx->v0.U32[0] = 0;
}

