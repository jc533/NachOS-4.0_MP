# note
## register 
- $2: v0
- $31: ra
- arg1 -- r4 a0
- arg2 -- r5 a1
- arg3 -- r6 a2
- arg4 -- r7 a3

Run->Onezinstruct:case OP_SYSCALL:
->ExceptionHandler SyscallException

