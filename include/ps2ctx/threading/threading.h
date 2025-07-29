#include "ps2ctx/threading/thread_param.h"

int Recomp_CreateThread(ps2_thread_param_t* param);
void Recomp_StartThread(int threadId, void* arg);
void Recomp_ReferThreadStatus(int threadId, ps2_thread_param_t* param);
