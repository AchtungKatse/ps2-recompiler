#include "ps2ctx/ps2_context.h"
#include "ps2ctx/threading/semaphor.h"
#include "ps2ctx/threading/threading.h"
#include "ps2ctx/threading/sema_param.h"
#include "ps2ctx/threading/thread_param.h"
#include "ps2ctx/sce/sif.h"
#include "common/logging.h"

#include <execinfo.h>
#include <unistd.h>

const char* syscall_names[];

// typedef enum ee_syscall {
//     SYSCALL_INVALID = 0x0,
//     SYSCALL_ResetEE = 0x1,
//     SYSCALL_SetGsCrt = 0x2,
//     SYSCALL_Exit = 0x4,
//     SYSCALL_CreateThread = 0x20,
//     SYSCALL_StartThread = 0x22,
//     SYSCALL_GetThreadId = 0x2f,
//     SYSCALL_ReferThreadStatus = 0x30,
//     SYSCALL_Sleep = 0x32,
//     SYSCALL_CreateSema = 0x40,
//     SYSCALL_DeleteSema = 0x41,
//     SYSCALL_SignalSema = 0x42,
//     SYSCALL_iSignalSema = 0x43,
//     SYSCALL_WaitSema = 0x44,
//     SYSCALL_PollSema = 0x45,
//     SYSCALL_iPollSema = 0x46,
//     SYSCALL_ReferSemaStatus = 0x47,
//     SYSCALL_iReferSemaStatus = 0x48,
//     SYSCALL_SetSyscall = 0x75,
//     SYSCALL_SifDmaStat = 0x76,
//     SYSCALL_SifSetDma = 0x77,
//     SYSCALL_SetSifRegister = 0x79,
//     SYSCALL_GetSifRegister = 0x7A,
// } ee_syscall_t;

typedef enum ee_syscall {
    SYSCALL_RFU000_FULLRESET                = 0x0,
    SYSCALL_RESET_EE                        = 0x1,
    SYSCALL_SET_GS_CRT                      = 0x2,
    SYSCALL_RFU003                          = 0x3,
    SYSCALL_EXIT                            = 0x4,
    SYSCALL_RFU005                          = 0x5,
    SYSCALL_LOAD_PS2_EXE                    = 0x6,
    SYSCALL_EXEC_PS2                        = 0x7,
    SYSCALL_RFU008                          = 0x8,
    SYSCALL_RFU009                          = 0x9,
    SYSCALL_ADD_SBUS_INTC_HANDLER           = 0xa,
    SYSCALL_REMOVE_SBUS_INTC_HANDLER        = 0xb,
    SYSCALL_INTERRUPT_2IOP                  = 0xc,
    SYSCALL_SET_VTLB_REFILL_HANDLER         = 0xd,
    SYSCALL_SET_VCOMMON_HANDLER             = 0xe,
    SYSCALL_SET_VINTERRUPT_HANDLER          = 0xf,
    SYSCALL_ADD_INTC_HANDLER                = 0x10,
    SYSCALL_REMOVE_INTC_HANDLER             = 0x11,
    SYSCALL_ADD_DMAC_HANDLER                = 0x12,
    SYSCALL_REMOVE_DMAC_HANDLER             = 0x13,
    SYSCALL_ENABLE_INTC                     = 0x14,
    SYSCALL_DISABLE_INTC                    = 0x15,
    SYSCALL_ENABLE_DMAC                     = 0x16,
    SYSCALL_DISABLE_DMAC                    = 0x17,
    SYSCALL_SET_ALARM                       = 0x18,
    SYSCALL_RELEASE_ALARM                   = 0x19,
    SYSCALL_IENABLE_INTC                    = 0x1a,
    SYSCALL_IDISABLE_INTC                   = 0x1b,
    SYSCALL_IENABLE_DMAC                    = 0x1c,
    SYSCALL_IDISABLE_DMAC                   = 0x1d,
    SYSCALL_ISET_ALARM                      = 0x1e,
    SYSCALL_IRELEASE_ALARM                  = 0x1f,
    SYSCALL_CREATE_THREAD                   = 0x20,
    SYSCALL_DELETE_THREAD                   = 0x21,
    SYSCALL_START_THREAD                    = 0x22,
    SYSCALL_EXIT_THREAD                     = 0x23,
    SYSCALL_EXIT_DELETE_THREAD              = 0x24,
    SYSCALL_TERMINATE_THREAD                = 0x25,
    SYSCALL_ITERMINATE_THREAD               = 0x26,
    SYSCALL_DISABLED_ISPATCH_THREAD         = 0x27,
    SYSCALL_ENABLED_ISPATCH_THREAD          = 0x28,
    SYSCALL_CHANGE_THREAD_PRIORITY          = 0x29,
    SYSCALL_ICHANGE_THREAD_PRIORITY         = 0x2a,
    SYSCALL_ROTATE_THREAD_READY_QUEUE       = 0x2b,
    SYSCALL_IROTATE_THREAD_READY_QUEUE      = 0x2c,
    SYSCALL_RELEASE_WAIT_THREAD             = 0x2d,
    SYSCALL_IRELEASE_WAIT_THREAD            = 0x2e,
    SYSCALL_GET_THREAD_ID                   = 0x2f,
    SYSCALL_REFER_THREAD_STATUS             = 0x30,
    SYSCALL_IREFER_THREAD_STATUS            = 0x31,
    SYSCALL_SLEEP_THREAD                    = 0x32,
    SYSCALL_WAKEUP_THREAD                   = 0x33,
    SYSCALL_IWAKEUP_THREAD                  = 0x34,
    SYSCALL_CANCEL_WAKEUP_THREAD            = 0x35,
    SYSCALL_ICANCEL_WAKEUP_THREAD           = 0x36,
    SYSCALL_SUSPEND_THREAD                  = 0x37,
    SYSCALL_ISUSPEND_THREAD                 = 0x38,
    SYSCALL_RESUME_THREAD                   = 0x39,
    SYSCALL_IRESUME_THREAD                  = 0x3a,
    SYSCALL_JOIN_THREAD                     = 0x3b,
    SYSCALL_RFU060                          = 0x3c,
    SYSCALL_RFU061                          = 0x3d,
    SYSCALL_END_OF_HEAP                     = 0x3e,
    SYSCALL_RFU063                          = 0x3f,
    SYSCALL_CREATE_SEMA                     = 0x40,
    SYSCALL_DELETE_SEMA                     = 0x41,
    SYSCALL_SIGNAL_SEMA                     = 0x42,
    SYSCALL_ISIGNAL_SEMA                    = 0x43,
    SYSCALL_WAIT_SEMA                       = 0x44,
    SYSCALL_POLL_SEMA                       = 0x45,
    SYSCALL_IPOLL_SEMA                      = 0x46,
    SYSCALL_REFER_SEMA_STATUS               = 0x47,
    SYSCALL_IREFER_SEMA_STATUS              = 0x48,
    SYSCALL_RFU073                          = 0x49,
    SYSCALL_SET_OSD_CONFIG_PARAM            = 0x4a,
    SYSCALL_GET_OSD_CONFIG_PARAM            = 0x4b,
    SYSCALL_GET_GSH_PARAM                   = 0x4c,
    SYSCALL_GET_GS_VPARAM                   = 0x4d,
    SYSCALL_SET_GSH_PARAM                   = 0x4e,
    SYSCALL_SET_GSV_PARAM                   = 0x4f,
    SYSCALL_RFU080_CREATE_EVENT_FLAG        = 0x50,
    SYSCALL_RFU081_DELETE_EVENT_FLAG        = 0x51,
    SYSCALL_RFU082_SET_EVENT_FLAG           = 0x52,
    SYSCALL_RFU083_ISET_EVENT_FLAG          = 0x53,
    SYSCALL_RFU084_CLEAR_EVENT_FLAG         = 0x54,
    SYSCALL_RFU085_ICLEAR_EVENT_FLAG        = 0x55,
    SYSCALL_RFU086_WAIT_EVNET_FLAG          = 0x56,
    SYSCALL_RFU087_POLL_EVNET_FLAG          = 0x57,
    SYSCALL_RFU088_IPOLL_EVNET_FLAG         = 0x58,
    SYSCALL_RFU089_REFER_EVENT_FLAG_STATUS  = 0x59,
    SYSCALL_RFU090_IREFER_EVENT_FLAG_STATUS = 0x5a,
    SYSCALL_RFU091                          = 0x5b,
    SYSCALL_ENABLE_INTC_HANDLER             = 0x5c,
    // SYSCALL_IENABLE_INTC_HANDLER            = 0x5c,
    SYSCALL_DISABLE_INTC_HANDLER            = 0x5d,
    // SYSCALL_IDISABLE_INTC_HANDLER           = 0x5d,
    SYSCALL_ENABLE_DMAC_HANDLER             = 0x5e,
    // SYSCALL_IENABLE_DMAC_HANDLER            = 0x5e,
    SYSCALL_DISABLE_DMAC_HANDLER            = 0x5f,
    // SYSCALL_IDISABLE_DMAC_HANDLER           = 0x5f,
    SYSCALL_KSEG0                           = 0x60,
    SYSCALL_ENABLE_CACHE                    = 0x61,
    SYSCALL_DISABLE_CACHE                   = 0x62,
    SYSCALL_GET_COP0                        = 0x63,
    SYSCALL_FLUSH_CACHE                     = 0x64,
    SYSCALL_CPU_CONFIG                      = 0x66,
    SYSCALL_IGET_COP0                       = 0x67,
    SYSCALL_IFLUSH_CACHE                    = 0x68,
    SYSCALL_ICPU_CONFIG                     = 0x6a,
    SYSCALL_SCE_SIF_STOP_DMA                = 0x6b,
    SYSCALL_SET_CPU_TIMER_HANDLER           = 0x6c,
    SYSCALL_SET_CPU_TIMER                   = 0x6d,
    SYSCALL_FORCE_READ                      = 0x6e,
    SYSCALL_FORCE_WRITE                     = 0x6f,
    SYSCALL_GS_GET_IMR                      = 0x70,
    // SYSCALL_IGS_GET_IMR                     = 0x70,
    SYSCALL_GS_PUT_IMR                      = 0x71,
    // SYSCALL_IGS_PUT_IMR                     = 0x71,
    SYSCALL_SET_PGIF_HANDLER                = 0x72,
    SYSCALL_SET_VSYNC_FLAG                  = 0x73,
    SYSCALL_RFU116                          = 0x74,
    SYSCALL_PRINT                           = 0x75,
    SYSCALL_SCE_SIF_DMA_STAT                = 0x76,
    SYSCALL_ISCE_SIF_DMA_STAT               = 0x76,
    SYSCALL_SCE_SIF_SET_DMA                 = 0x77,
    // SYSCALL_ISCE_SIF_SET_DMA                = 0x77,
    SYSCALL_SCE_SIF_SET_DCHAIN              = 0x78,
    // SYSCALL_ISCE_SIF_SET_DCHAIN             = 0x78,
    SYSCALL_SCE_SIF_SET_REG                 = 0x79,
    SYSCALL_SCE_SIF_GET_REG                 = 0x7a,
    SYSCALL_EXE_COSD                        = 0x7b,
    SYSCALL_DEC_I2_CALL                     = 0x7c,
    SYSCALL_PS_MODE                         = 0x7d,
    SYSCALL_MACHINE_TYPE                    = 0x7e,
    SYSCALL_GET_MEMORY_SIZE                 = 0x7f,
} ee_syscall_t;

void unimplemented_syscall(ee_syscall_t syscall) {
    void* backtraces[0x10];
    u32 size = backtrace(backtraces, 0x10);

    log_warn("Failed to run unimplemented syscall '%s' (0x%x)\n", syscall_names[syscall], syscall);
    backtrace_symbols_fd(backtraces, size, STDERR_FILENO);
}

void EESyscall(u8 index) {
    u32 reg = 0;
    switch (index) {
        default:
            unimplemented_syscall(index);
            break;
        case SYSCALL_CREATE_THREAD:
            ctx->v0.U32[0] = Recomp_CreateThread((ps2_thread_param_t*)FindRam(ctx->a0.U32[0]));
            break;
        case SYSCALL_START_THREAD:
            Recomp_StartThread(ctx->a0.U32[0], (void*)ctx->a1.U64[0]);
            break;
        case SYSCALL_GET_THREAD_ID:
            ctx->v0.U32[0] = EEThreadID;
            break;
        case SYSCALL_REFER_THREAD_STATUS: 
            Recomp_ReferThreadStatus(ctx->a0.U32[0], (ps2_thread_param_t*)ctx->a1.U64[0]);
            break;
        case SYSCALL_SLEEP_THREAD:
            while (true)
                sleep(~0u);
            break;
        case SYSCALL_CREATE_SEMA:
            ctx->v0.U32[0] = Kernel_CreateSema((SemaParam*)FindRam(ctx->a0.U32[0]));
            break;
        case SYSCALL_DELETE_SEMA:
            ctx->v0.U32[0] = Kernel_DeleteSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_SIGNAL_SEMA:
            ctx->v0.U32[0] = Kernel_SignalSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_ISIGNAL_SEMA:
            ctx->v0.U32[0] = Kernel_iSignalSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_WAIT_SEMA:
            ctx->v0.U32[0] = Kernel_WaitSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_POLL_SEMA:
            ctx->v0.U32[0] = Kernel_PollSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_IPOLL_SEMA:
            ctx->v0.U32[0] = Kernel_PollSema(ctx->a0.U32[0]);
            break;
        case SYSCALL_REFER_SEMA_STATUS:
            ctx->v0.U32[0] = Kernel_ReferSemaStatus(ctx->a0.U32[0], FindRam(ctx->a1.U32[0]));
            break;
        case SYSCALL_IREFER_SEMA_STATUS:
            ctx->v0.U32[0] = Kernel_iReferSemaStatus(ctx->a0.U32[0], FindRam(ctx->a1.U32[0]));
            break;
        case SYSCALL_SCE_SIF_SET_DMA:
            ctx->v0.U32[0] = Kernel_SifSetDma(FindRam(ctx->a0.U32[0]), ctx->a1.U32[0]);
            break;
        case SYSCALL_SCE_SIF_SET_REG: 
            *(u64*)FindRam(ctx->a0.U32[0] * 0x10 + 0x1000F200) = ctx->a1.U64[0];
            break;
        case SYSCALL_SCE_SIF_GET_REG:
            reg = ctx->a0.U32[0] &= ~0x80000000;

            if (reg == 4) {
                ctx->v0.U32[0] =    (1 << 1) |
                    (1 << 8) | 
                    (1 << 17) | // SIFCMD Initialized (sure, why not?)
                    (0xf << 28);
                break;
            }
            /*if (ctx->a0.U32[0] & 0x80000000) // Some kind of data request*/
            /*    return;*/
            ctx->v0.U64[0] = *(u64*)FindRam(reg * 0x10 + 0x1000F200);
            break;
    }
}

const char* syscall_names[0x7f] = {
    "RFU000_FullReset",
    "ResetEE",
    "SetGsCrt",
    "RFU003",
    "Exit",
    "RFU005",
    "LoadPS2Exe",
    "ExecPS2",
    "RFU008",
    "RFU009",
    "AddSbusIntcHandler",
    "RemoveSbusIntcHandler",
    "Interrupt2Iop",
    "SetVTLBRefillHandler",
    "SetVCommonHandler",
    "SetVInterruptHandler",
    "AddIntcHandler",
    "RemoveIntcHandler",
    "AddDmacHandler",
    "RemoveDmacHandler",
    "_EnableIntc",
    "_DisableIntc",
    "_EnableDmac",
    "_DisableDmac",
    "_SetAlarm",
    "_ReleaseAlarm",
    "_iEnableIntc",
    "_iDisableIntc",
    "_iEnableDmac",
    "_iDisableDmac",
    "_iSetAlarm",
    "_iReleaseAlarm",
    "CreateThread",
    "DeleteThread",
    "StartThread",
    "ExitThread",
    "ExitDeleteThread",
    "TerminateThread",
    "iTerminateThread",
    "DisableDispatchThread",
    "EnableDispatchThread",
    "ChangeThreadPriority",
    "iChangeThreadPriority",
    "RotateThreadReadyQueue",
    "iRotateThreadReadyQueue",
    "ReleaseWaitThread",
    "iReleaseWaitThread",
    "GetThreadId",
    "ReferThreadStatus",
    "iReferThreadStatus",
    "SleepThread",
    "WakeupThread",
    "iWakeupThread",
    "CancelWakeupThread",
    "iCancelWakeupThread",
    "SuspendThread",
    "iSuspendThread",
    "ResumeThread",
    "iResumeThread",
    "JoinThread",
    "RFU060",
    "RFU061",
    "EndOfHeap",
    "RFU063",
    "CreateSema",
    "DeleteSema",
    "SignalSema",
    "iSignalSema",
    "WaitSema",
    "PollSema",
    "iPollSema",
    "ReferSemaStatus",
    "iReferSemaStatus",
    "RFU073",
    "SetOsdConfigParam",
    "GetOsdConfigParam",
    "GetGsHParam",
    "GetGsVParam",
    "SetGsHParam",
    "SetGsVParam",
    "RFU080_CreateEventFlag",
    "RFU081_DeleteEventFlag",
    "RFU082_SetEventFlag",
    "RFU083_iSetEventFlag",
    "RFU084_ClearEventFlag",
    "RFU085_iClearEventFlag",
    "RFU086_WaitEvnetFlag",
    "RFU087_PollEvnetFlag",
    "RFU088_iPollEvnetFlag",
    "RFU089_ReferEventFlagStatus",
    "RFU090_iReferEventFlagStatus",
    "RFU091",
    "EnableIntcHandler",
    // "iEnableIntcHandler",
    "DisableIntcHandler",
    // "iDisableIntcHandler",
    "EnableDmacHandler",
    // "iEnableDmacHandler",
    "DisableDmacHandler",
    // "iDisableDmacHandler",
    "KSeg0",
    "EnableCache",
    "DisableCache",
    "GetCop0",
    "FlushCache",
    "CpuConfig",
    "iGetCop0",
    "iFlushCache",
    "iCpuConfig",
    "sceSifStopDma",
    "SetCPUTimerHandler",
    "SetCPUTimer",
    "ForceRead",
    "ForceWrite",
    "GsGetIMR",
    // "iGsGetIMR",
    "GsPutIMR",
    // "iGsPutIMR",
    "SetPgifHandler",
    "SetVSyncFlag",
    "RFU116",
    "print",
    "sceSifDmaStat",
    "isceSifDmaStat",
    "sceSifSetDma",
    // "isceSifSetDma",
    "sceSifSetDChain",
    // "isceSifSetDChain",
    "sceSifSetReg",
    "sceSifGetReg",
    "ExecOSD",
    "Deci2Call",
    "PSMode",
    "MachineType",
    "GetMemorySize",
};
