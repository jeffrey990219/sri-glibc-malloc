/*
 * Copyright (C) 2016  SRI International
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if !defined(NDEBUG) && defined(SRI_MALLOC_LOG)

#define _GNU_SOURCE

#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>


/* a quick 'n dirty version of mhook */

#include "debug.h"

static int logfd = -1;

static const char hex[16] = "0123456789abcdef";

static const char logfile[] = "/tmp/frolloc.log";


enum mhooklen { MALLOCLEN = 58, FREELEN = 39, REALLOCLEN = 77 };

enum mhookargs { MALLOCARGS = 3, FREEARGS  = 2, REALLOCARGS = 4 };


// A set of logging functions that don't call malloc() and friends....
// Code assumes LP64 model, and sizeof(size_t) <= sizeof(uintptr_t)

static void storehexstring(char *buf, uintptr_t val)
{
  int pos = 2 * sizeof(val) - 1;
  int t;
  while(val > 0 && pos >= 0) {
    t = val & 0xF;
    buf[pos--] = hex[t];
    val >>= 4;
  }
}

void log_init (void)
{
  int fd ;
  unlink(logfile);
  fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0600);
  if (fd >= 0) {
    logfd = fd;
  }
}

void log_end(void){
  if(logfd >= 0){
    fsync(logfd);
    close(logfd);
    logfd = -1;
  }
}

static void _writelogentry(char func, size_t size1, size_t size2, void *p, void *q, void *caller)
{
  char buffer[] = { ' ', ' ', '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
		         ' ', '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
		         ' ', '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
		         ' ', '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
		         ' ', '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
		    '\n' };
  int sz = sizeof(buffer) - 1;
  int rcode;
  buffer[0] = func;
  switch (func) {
  case 'm':
    storehexstring(&buffer[4],  (uintptr_t)p);
    storehexstring(&buffer[23], (uintptr_t)size1);
    storehexstring(&buffer[42], (uintptr_t)caller);
    sz = MALLOCLEN;
    break;
  case 'f':
    storehexstring(&buffer[4],  (uintptr_t)p);
    storehexstring(&buffer[23], (uintptr_t)caller);
    sz = FREELEN;
    break;
  case 'r':
    storehexstring(&buffer[4],  (uintptr_t)q);
    storehexstring(&buffer[23], (uintptr_t)p);
    storehexstring(&buffer[42], (uintptr_t)size1);
    storehexstring(&buffer[61], (uintptr_t)caller);
    sz = REALLOCLEN;
    break;
  default:
    sz = 5;
  }
  buffer[sz] = '\n';
  
  if(logfd < 0){
    return;
  }
  rcode = write(logfd, buffer, sz+1);
  if(rcode != sz+1) {
    if(rcode < 0){
      if(errno == EINTR){
	exit(3);
      } else if(errno == EBADF){
	exit(5);
      } else {
	exit(errno);
      }
    } else  {
      exit(7);
    }
  }
}


void log_malloc(void* val, size_t size)
{
  _writelogentry('m', size, 0, val, NULL, (void*)pthread_self());
}
void log_realloc(void* val, void* oval, size_t size)
{
  _writelogentry('r', size, 0, oval, val, (void*)pthread_self());
}
void log_free(void* val)
{
  _writelogentry('f', 0, 0, val, NULL, (void*)pthread_self());
}


#endif