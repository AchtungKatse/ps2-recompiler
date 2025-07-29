#pragma once
#include <semaphore.h>

typedef struct {
    int count, max_count, init_count, wait_threads;
    int attr, option;
} SemaParam;

/*typedef struct {*/
/*    struct TCB *prev;*/
/*    struct TCB *next;*/
/*    int status;*/
/*    void *func;*/
/*    void *current_stack;*/
/*    void *gp_reg;*/
/*    short current_priority;*/
/*    short init_priority;*/
/*    int wait_type; //0=not waiting, 1=sleeping, 2=waiting on semaphore*/
/*    int sema_id;*/
/*    int wakeup_count;*/
/*    int attr;*/
/*    int option;*/
/*    void *_func; //???*/
/*    int argc;*/
/*    char **argv;*/
/*    void *initial_stack;*/
/*    int stack_size;*/
/*    int *root; //function to return to when exiting thread?*/
/*    void *heap_base;*/
/*} TCB;  */
