# MP3
## trace code
Trace code: Explain the purposes and details of the following 6 code paths to
understand how nachos manages the lifecycle of a process (or thread) as described in the Diagram of Process State in our lecture slides (chp.3 p.8).

1-1. New→Ready
- Kernel::ExecAll()
[](./code/threads/kernel.cc#L255)

- Kernel::Exec(char*)
[](./code/threads/kernel.cc#L263)

- Thread::Fork(VoidFunctionPtr, void*)
[](./code/threads/thread.cc#L91)

- Thread::StackAllocate(VoidFunctionPtr, void*)
[](./code/threads/thread.cc#L301)

- Scheduler::ReadyToRun(Thread*)
[](./code/threads/xscheduler.cc#L55)

1-2. Running→Ready
- Machine::Run()
[](./code/machine/mipssim.cc#L54)

- Interrupt::OneTick()
[](./code/machine/interrupt.cc#L145)

- Thread::Yield()
[](./code/threads/thread.cc#L200)

- Scheduler::FindNextToRun()
[](./code/threads/scheduler.cc#L72)

- Scheduler::ReadyToRun(Thread*)
[](./code/threads/scheduler.cc#L55)

- Scheduler::Run(Thread*, bool)
[](./code/threads/scheduler.cc#L99)

1-3. Running→Waiting (Note: only need to consider console output as an example)
- SynchConsoleOutput::PutChar(char)
[](./code/userprog/synchconsole.cc#L93)

- Semaphore::P()
[](./code/threads/synch.cc#L74)

- List<T>::Append(T)
[](./code/lib/list.cc#L70)

- Thread::Sleep(bool)
[](./code/threads/thread.cc#L236)

- Scheduler::FindNextToRun()
[](./code/threads/scheduler.cc#L72)

- Scheduler::Run(Thread*, bool)
[](./code/threads/scheduler.cc#L99)

1-4. Waiting→Ready (Note: only need to consider console output as an example)
- Semaphore::V()
[](./code/threads/synch.cc#L100)
- Scheduler::ReadyToRun(Thread*)
[](./code/threads/scheduler.cc#L96)
1-5. Running→Terminated (Note: start from the Exit system call is called)
- ExceptionHandler(ExceptionType) case SC_Exit
[](./code/userprog/exception.cc#L50)
- Thread::Finish()
[](./code/threads/thread.cc#L280)
- Thread::Sleep(bool)
[](./code/threads/thread.cc#L250)
- Scheduler::FindNextToRun()
[](./code/threads/scheduler.cc#L125)
- Scheduler::Run(Thread*, bool)
[](./code/threads/scheduler.cc#L183)
1-6. Ready→Running
- Scheduler::FindNextToRun()
[](./code/threads/scheduler.cc#L125)
- Scheduler::Run(Thread*, bool)
[](./code/threads/scheduler.cc#L183)
- SWITCH(Thread*, Thread*)
[](./code/threads/switch.S#L295)
(depends on the previous process state, e.g.,
[New,Running,Waiting]→Ready→Running)

- for loop in Machine::Run()
[](./code/machine/mipssim.cc#L54)