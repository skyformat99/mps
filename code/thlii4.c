/*  impl.c.thlii3: Threads Manager for Intel x86 systems with LinuxThreads
 *
 *  $Id$
 *  Copyright (c) 2001 Ravenbrook Limited.
 *
 * .purpose: This is a pthreads implementation of the threads manager.
 * This implements impl.h.th.
 *
 * .design: See design.mps.thread-manager.
 *
 * .thread.id: The thread id is used to identify the current thread.
 *
 * ASSUMPTIONS
 *
 * .error.resume: PThreadextResume is assumed to succeed unless the thread
 * has been destroyed.
 * .error.suspend: PThreadextSuspend is assumed to succeed unless the thread
 * has been destroyed. In this case, the suspend context is set to NULL;
 *
 * .stack.full-descend:  assumes full descending stack.
 * i.e. stack pointer points to the last allocated location;
 * stack grows downwards.
 *
 * .stack.below-bottom: it's legal for the stack pointer to be at a
 * higher address than the registered bottom of stack. This might
 * happen if the stack of another thread doesn't contain any frames
 * belonging to the client language. In this case, the stack should
 * not be scanned.
 *
 * .stack.align: assume roots on the stack are always word-aligned,
 * but don't assume that the stack pointer is necessarily
 * word-aligned at the time of reading the context of another thread.
 *
 * .sp: The stack pointer in the context is ESP.
 * .context.regroots: The root regs are EDI, ESI, EBX, EDX, ECX, EAX are
 * assumed to be recorded in the context at pointer-aligned boundaries.
 */

#include "prmcli.h"
#include "mpm.h"

#if !defined(MPS_OS_LI) || !defined(MPS_ARCH_I4)
#error "Compiling thlii4 when MPS_OS_LI or MPS_ARCH_I4 not defined."
#endif

#include <pthread.h>
#include "pthrdext.h"

SRCID(thlii4, "$Id$");


/* ThreadStruct -- thread desriptor */

typedef struct ThreadStruct {    /* PThreads thread structure */
  Sig sig;                       /* design.mps.sig */
  Serial serial;                 /* from arena->threadSerial */
  Arena arena;                   /* owning arena */
  RingStruct arenaRing;          /* threads attached to arena */
  PThreadextStruct thrextStruct; /* PThreads extension */
  pthread_t id;                  /* Pthread object of thread */
  MutatorFaultContext mfc;       /* Context if thread is suspended */
} ThreadStruct;


/* ThreadCheck -- check a thread */

Bool ThreadCheck(Thread thread)
{
  CHECKS(Thread, thread);
  CHECKU(Arena, thread->arena);
  CHECKL(thread->serial < thread->arena->threadSerial);
  CHECKL(RingCheck(&thread->arenaRing));
  CHECKD(PThreadext, &thread->thrextStruct);
  return TRUE;
}

Bool ThreadCheckSimple(Thread thread)
{
  CHECKS(Thread, thread);
  return TRUE;
}


/* ThreadRegister -- register a thread with an arena */

Res ThreadRegister(Thread *threadReturn, Arena arena)
{
  Res res;
  Thread thread;
  void *p;

  AVER(threadReturn != NULL);
  AVERT(Arena, arena);

  res = ControlAlloc(&p, arena, sizeof(ThreadStruct),
                     /* withReservoirPermit */ FALSE);
  if(res != ResOK)
    return res;
  thread = (Thread)p;

  thread->id = pthread_self();

  RingInit(&thread->arenaRing);

  thread->sig = ThreadSig;
  thread->serial = arena->threadSerial;
  ++arena->threadSerial;
  thread->arena = arena;
  thread->mfc = NULL;

  PThreadextInit(&thread->thrextStruct, thread->id);

  AVERT(Thread, thread);

  RingAppend(ArenaThreadRing(arena), &thread->arenaRing);

  *threadReturn = thread;
  return ResOK;
}


/* ThreadDeregister -- deregister a thread from an arena */

void ThreadDeregister(Thread thread, Arena arena)
{
  AVERT(Thread, thread);
  AVERT(Arena, arena);

  RingRemove(&thread->arenaRing);

  thread->sig = SigInvalid;

  RingFinish(&thread->arenaRing);

  PThreadextFinish(&thread->thrextStruct);

  ControlFree(arena, thread, sizeof(ThreadStruct));
}


/*  mapThreadRing -- map over threads on ring calling a function on each one
 *                   except the current thread
 */

static void mapThreadRing(Ring threadRing, void (*func)(Thread))
{
  Ring node, next;
  pthread_t self;

  AVERT(Ring, threadRing);

  self = pthread_self();
  RING_FOR(node, threadRing, next) {
    Thread thread = RING_ELT(Thread, arenaRing, node);
    AVERT(Thread, thread);
    if(! pthread_equal(self, thread->id)) /* .thread.id */
      (*func)(thread);
  }
}


/* ThreadRingSuspend -- suspend all threads on a ring, expect the current one */


static void threadSuspend(Thread thread)
{
  /* .error.suspend */
  /* In the error case (PThreadextSuspend returning ResFAIL), we */
  /* assume the thread has been destroyed. */
  /* In which case we simply continue. */
  Res res;
  res = PThreadextSuspend(&thread->thrextStruct, &thread->mfc);
  if(res != ResOK)
    thread->mfc = NULL;
}



void ThreadRingSuspend(Ring threadRing)
{
  mapThreadRing(threadRing, threadSuspend);
}


/* ThreadRingResume -- resume all threads on a ring (expect the current one) */


static void threadResume(Thread thread)
{
  /* .error.resume */
  /* If the previous suspend failed (thread->mfc == NULL), */
  /* or in the error case (PThreadextResume returning ResFAIL), */
  /* assume the thread has been destroyed. */
  /* In which case we simply continue. */
  if(thread->mfc != NULL) {
    (void)PThreadextResume(&thread->thrextStruct);
    thread->mfc = NULL;
  }
}

void ThreadRingResume(Ring threadRing)
{
  mapThreadRing(threadRing, threadResume);
}


/* ThreadRingThread -- return the thread at the given ring element */

Thread ThreadRingThread(Ring threadRing)
{
  Thread thread;
  AVERT(Ring, threadRing);
  thread = RING_ELT(Thread, arenaRing, threadRing);
  AVERT(Thread, thread);
  return thread;
}


/* ThreadArena -- get the arena of a thread
 *
 * Must be thread-safe.  See design.mps.interface.c.thread-safety.
 */

Arena ThreadArena(Thread thread)
{
  /* Can't check thread as that would not be thread-safe. */
  return thread->arena;
}


/* ThreadScan -- scan the state of a thread (stack and regs) */

Res ThreadScan(ScanState ss, Thread thread, void *stackBot)
{
  pthread_t self;
  Res res;

  AVERT(Thread, thread);
  self = pthread_self();
  if(pthread_equal(self, thread->id)) {
    /* scan this thread's stack */
    res = StackScan(ss, stackBot);
    if(res != ResOK)
      return res;
  } else {
    struct sigcontext *scp;
    Addr *stackBase, *stackLimit, stackPtr;

    scp = thread->mfc->scp;
    if(scp == NULL) {
      /* .error.suspend */
      /* We assume that the thread must have been destroyed. */
      /* We ignore the situation by returning immediately. */
      return ResOK;
    }

    stackPtr  = (Addr)scp->esp;   /* .i3.sp */
    /* .stack.align */
    stackBase  = (Addr *)AddrAlignUp(stackPtr, sizeof(Addr));
    stackLimit = (Addr *)stackBot;
    if (stackBase >= stackLimit)
      return ResOK;    /* .stack.below-bottom */

    /* scan stack inclusive of current sp and exclusive of
     * stackBot (.stack.full-descend)
     */
    res = TraceScanAreaTagged(ss, stackBase, stackLimit);
    if(res != ResOK)
      return res;

    /* (.context.regroots)
     * This scans the root registers (.context.regroots).  It also
     * unecessarily scans the rest of the context.  The optimisation
     * to scan only relevent parts would be machine dependent.
     */
    res = TraceScanAreaTagged(ss, (Addr *)scp,
           (Addr *)((char *)scp + sizeof(*scp)));
    if(res != ResOK)
      return res;
  }

  return ResOK;
}


/* ThreadDescribe -- describe a thread */

Res ThreadDescribe(Thread thread, mps_lib_FILE *stream)
{
  Res res;

  res = WriteF(stream,
               "Thread $P ($U) {\n", (WriteFP)thread, (WriteFU)thread->serial,
               "  arena $P ($U)\n",
               (WriteFP)thread->arena, (WriteFU)thread->arena->serial,
               "  id $U\n",          (WriteFU)thread->id,
               "} Thread $P ($U)\n", (WriteFP)thread, (WriteFU)thread->serial,
               NULL);
  if(res != ResOK)
    return res;

  return ResOK;
}
