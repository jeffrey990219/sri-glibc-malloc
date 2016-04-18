#ifndef __MAGED_INTERNALS_H__
#define __MAGED_INTERNALS_H__

#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "atomic.h"
#include "queue.h"

struct Descriptor;
typedef struct Descriptor descriptor;
struct Procheap;
typedef struct Procheap procheap;

#define TYPE_SIZE	8
#define PTR_SIZE	sizeof(void*)
#define HEADER_SIZE	(TYPE_SIZE + PTR_SIZE)

#define LARGE		0
#define SMALL		1

#define	PAGESIZE	4096
#define SBSIZE		(16 * PAGESIZE)
#define DESCSBSIZE	(1024 * sizeof(descriptor))
#define SB_ALIGNMENT    SBSIZE

#define ACTIVE		0
#define FULL		1
#define PARTIAL		2
#define EMPTY		3

#define	MAXCREDITS	64    // 2^(bits for credits in active)
#define GRANULARITY	16    // sri: for x86_64 alignment we require 16 NOT 8

// sri: bigger than this we mmap. code is not yet 
// parametric in this. change it and the code needs
// changing too.
#define MAX_BLOCK_SIZE  2048  
                              

/* We need to squeeze this in 64-bits, but conceptually
 * this is the case:
 *	descriptor* DescAvail;
 */
typedef struct {
  uintptr_t 	DescAvail;
  uint64_t      tag;
} descriptor_queue;

/* Superblock descriptor structure. We bumped avail and count 
 * to 24 bits to support larger superblock sizes. */
typedef struct {
	unsigned long long 	avail:24,count:24, state:2, tag:14;
} anchor;

struct Descriptor {
	struct queue_elem_t	lf_fifo_queue_padding;
	volatile anchor		Anchor;
	descriptor*		Next;
	void*			sb;		// pointer to superblock
	procheap*		heap;		// pointer to owner procheap
	unsigned int		sz;		// block size
	unsigned int		maxcount;	// superblock size / sz
};

typedef struct {
	lf_fifo_queue_t		Partial;	// initially empty
	unsigned int		sz;		// block size
	unsigned int		sbsize;		// superblock size
} sizeclass;

typedef struct {
  uintptr_t	ptr;
  uint64_t      credits;
} active;

struct Procheap {
	volatile active		Active;		// initially NULL
	volatile descriptor*	Partial;	// initially NULL
	sizeclass*		sc;		// pointer to parent sizeclass
};

static inline long min(long a, long b)
{
  return a < b ? a : b;
}

static inline long max(long a, long b)
{
  return a > b ? a : b;
}

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#endif	/* __MAGED_INTERNALS_H__ */