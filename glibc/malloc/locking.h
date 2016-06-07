#ifdef SRI_MALLOC_LOG
#include "debug.h"

#ifdef SRI_VALGRIND
#include "valgrind/helgrind.h"
#endif

#include <atomic.h>

static volatile int tid_counter = 0;
static __thread  int tid = -1;


static inline void LOCK_ARENA(mstate av, int site){
  int self;
  if(tid == -1){
    tid = catomic_exchange_and_add (&tid_counter, 1);
  }
  self = tid;
  
#ifdef SRI_VALGRIND
  VALGRIND_HG_MUTEX_LOCK_PRE(&(av->mutex), 0);
#endif

  (void)mutex_lock(&(av->mutex));
  log_lock_event(LOCK_ACTION, av, av->arena_index, site, tid);

#ifdef SRI_VALGRIND
  VALGRIND_HG_MUTEX_LOCK_POST(&(av->mutex));
#endif

}


static inline void UNLOCK_ARENA(mstate av, int site){

#ifdef SRI_VALGRIND
  VALGRIND_HG_MUTEX_UNLOCK_PRE(&(av->mutex));
#endif

  (void)mutex_unlock(&(av->mutex));
  log_lock_event(UNLOCK_ACTION, av, av->arena_index, site, tid);

#ifdef SRI_VALGRIND
  VALGRIND_HG_MUTEX_UNLOCK_POST(&(av->mutex));
#endif


}

#else

static inline void LOCK_ARENA(mstate av, int site){
  (void)mutex_lock(&(av->mutex));
}
static inline void UNLOCK_ARENA(mstate av, int site){
  (void)mutex_unlock(&(av->mutex));
}

#endif