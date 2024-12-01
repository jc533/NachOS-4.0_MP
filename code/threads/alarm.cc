// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "alarm.h"

#include "copyright.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom) {
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as
//	if the interrupted thread called Yield at the point it is
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice
//      if we're currently running something (in other words, not idle).
//----------------------------------------------------------------------

void Alarm::CallBack() {
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();

    int type;
    if(kernel->currentThread->priority<50){
        type = 3;
    }else if (kernel->currentThread->priority<100){
        type = 2;
    }else{
        type = 1;
    }
    // yield for preemptive
    if(status == RUNNING){
        kernel->currentThread->T = kernel->stats->totalTicks - kernel->currentThread->enterTick;
    }
    bool yield = FALSE;
    Thread* cur = kernel->currentThread;
    if(!kernel->scheduler->L1->IsEmpty()){
        if(type==1){
            Thread* minSJF = kernel->scheduler->L1->Front();
            double L1Remain = minSJF->burstTime-minSJF->T;
            double curRemain = cur->burstTime - cur->T;
            if(L1Remain<curRemain){
                yield = TRUE;
            }else if(L1Remain==curRemain && minSJF->getID()<cur->getID()){
                yield = TRUE;
            }
        }else if(type>=2){
            yield = TRUE;
        }
    else if(!kernel->scheduler->L2->IsEmpty()){
        if(type==3){
            yield = TRUE;
        }
    }else if (type==3 && status != IdleMode) {
        yield = TRUE;
    }
    if(yield){
        interrupt->YieldOnReturn();
    }

    kernel->scheduler->Aging();
}
