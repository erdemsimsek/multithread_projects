Calling sbrk(0) gives the current address of program break.
Calling sbrk(x) with a positive value increments brk by x bytes, as a result allocating memory.
Calling sbrk(-x) with a negative value decrements brk by x bytes, as a result releasing memory.