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
[](./code/threads/scheduler.cc#L55)


nonpreemptive
1-3. Running→Waiting (Note: only need to consider console output as an example)
SynchConsoleOutput::PutChar(char)
      Semaphore::P()
   List<T>::Append(T)
   Thread::Sleep(bool)
Scheduler::FindNextToRun()
Scheduler::Run(Thread*, bool)


1-5. Running→Terminated (Note: start from the Exit system call is called)
ExceptionHandler(ExceptionType) case SC_Exit
   Thread::Finish()
   Thread::Sleep(bool)
Scheduler::FindNextToRun()
Scheduler::Run(Thread*, bool)


preemptive
1-4. Waiting→Ready (Note: only need to consider console output as an example)
   Semaphore::V()
Scheduler::ReadyToRun(Thread*)

1-2. Running→Ready
Machine::Run()
Interrupt::OneTick()
Thread::Yield()
Scheduler::FindNextToRun()
Scheduler::ReadyToRun(Thread*)
Scheduler::Run(Thread*, bool)











1-6. Ready→Running
Scheduler::FindNextToRun()
Scheduler::Run(Thread*, bool)
SWITCH(Thread*, Thread*)
(depends on the previous process state, e.g., [New,Running,Waiting]→Ready→Running)
for loop in Machine::Run()
Note: switch.S contains the instructions to perform context switch. You must understand and describe the purpose of these instructions and explain the x86 instructions in your report. (You can try to understand the x86 instructions first. Appendix C and the MIPS version equivalent to x86 can get a lot of help.)
