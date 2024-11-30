// scheduler.cc
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would
//	end up calling FindNextToRun(), and that would put us in an
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "scheduler.h"

#include "copyright.h"
#include "debug.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

Scheduler::Scheduler() {
    // readyList = new List<Thread *>;
    L1 = new List<Thread *>;
    L2 = new List<Thread *>;
    L3 = new List<Thread *>;
    toBeDestroyed = NULL;
}

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler() {
    // delete readyList;
    delete L1;
    delete L2;
    delete L3;
}

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void Scheduler::ReadyToRun(Thread *thread) {
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());
    // cout << "Putting thread on ready list: " << thread->getName() << endl ;
    thread->setStatus(READY);
    // readyList->Append(thread);
    if(thread->priority <= 49){
        L3->Append(thread); // round rodbin?
    }else if(thread->priority <= 99){
        L2->Append(thread);//done nonpreemptive
    }else{
        L1->Append(thread);// preemptive how?
    }
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun() {
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    // if (readyList->IsEmpty()) {
    //     return NULL;
    // } else {
    //     return readyList->RemoveFront();
    // }
    bool L1_empty = L1->IsEmpty();
    bool L2_empty = L2->IsEmpty();
    bool L3_empty = L3->IsEmpty();

    if(L1_empty && L2_empty && L3_empty){
        return NULL;
    }else{
        if(!L1_empty){
            return L1->RemoveFront();
        }else if(!L2_empty){
            return L2->RemoveFront();
        }else{
            return L3->RemoveFront();
        }
    }

}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void Scheduler::Run(Thread *nextThread, bool finishing) {
    Thread *oldThread = kernel->currentThread;

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {  // mark that we need to delete current thread
        ASSERT(toBeDestroyed == NULL);
        toBeDestroyed = oldThread;
    }

    if (oldThread->space != NULL) {  // if this thread is a user program,
        oldThread->SaveUserState();  // save the user's CPU registers
        oldThread->space->SaveState();
    }

    oldThread->CheckOverflow();  // check if the old thread
                                 // had an undetected stack overflow

    kernel->currentThread = nextThread;  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running

    DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);

    // we're back, running oldThread

    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyed();  // check if thread we were running
                           // before this one has finished
                           // and needs to be cleaned up

    if (oldThread->space != NULL) {     // if there is an address space
        oldThread->RestoreUserState();  // to restore, do it.
        oldThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void Scheduler::CheckToBeDestroyed() {
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
        toBeDestroyed = NULL;
    }
}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void Scheduler::Print() {
    cout << "Ready list contents:\n";
    // readyList->Apply(ThreadPrint);
    L1->Apply(ThreadPrint);
    L2->Apply(ThreadPrint);
    L3->Apply(ThreadPrint);
}


void Scheduler::Aging(){
    for(int i=0;i<L3->NumInList();i++){
        int waitTick = kernel->stats->totalTicks - L3->Front()->enterTick;
        if(waitTick>1500){
            L3->Front()->priority += 10;
            L3->Front()->enterTick = kernel->stats->totalTicks;
            if(L3->Front()->priority > 49){
                L2->Append(L3->RemoveFront());
            }else{
                L3->Append(L3->RemoveFront());
            }
        } 
    }
    for(int i=0;i<L2->NumInList();i++){
        int waitTick = kernel->stats->totalTicks - L2->Front()->enterTick;
        if(waitTick>1500){
            L2->Front()->priority += 10;
            L2->Front()->enterTick = kernel->stats->totalTicks;
            if(L2->Front()->priority > 99){
                L1->Append(L2->RemoveFront());
            }else{
                L2->Append(L2->RemoveFront());
            }
        } 
    }
    // don't know L1 need to aging or not
    
}