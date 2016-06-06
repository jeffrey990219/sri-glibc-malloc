#if SRI_MALLOC_LOG
#include "debug.h"
#include "valgrind/helgrind.h"

#include <atomic.h>

static volatile int owners[16];

static volatile int tid_counter = 0;
static __thread  int tid = -1;


static inline void LOCK_ARENA(mstate av, int site){
  int self;
  if(tid == -1){
    tid = catomic_exchange_and_add (&tid_counter, 1);
  }
  self = tid;
  
  VALGRIND_HG_MUTEX_LOCK_PRE(&(av->mutex), 0);
  (void)mutex_lock(&(av->mutex));
  VALGRIND_HG_MUTEX_LOCK_POST(&(av->mutex));
  //allow for recursive locking
  //assert((owners[av->arena_index] == 0) || (owners[av->arena_index] == self));
  owners[av->arena_index] = self;
  
  //log_lock_event(LOCK_ACTION, av, av->arena_index, site, tid);
}


static inline void UNLOCK_ARENA(mstate av, int site){
  //int self = tid;
  //allow for recursive locking
  //assert((owners[av->arena_index] == 0) || (owners[av->arena_index] == self)); 
  owners[av->arena_index] = 0;
  VALGRIND_HG_MUTEX_UNLOCK_PRE(&(av->mutex));
  (void)mutex_unlock(&(av->mutex));
  VALGRIND_HG_MUTEX_UNLOCK_POST(&(av->mutex));

  //log_lock_event(UNLOCK_ACTION, av, av->arena_index, site, tid);
}

#else

#define LOCK_ARENA(AV, S)   ((void)mutex_lock(&(AV->mutex)))

#define UNLOCK_ARENA(AV, S) ((void)mutex_unlock(&(AV->mutex)))

#endif
