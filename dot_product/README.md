# Dot Product C program

This application contain a simple dot product function between 2 byte-vectors of 1000 elements.

The innermost kernel is written in assembler to give you the whole control of the instructions
executed by the core.

For a quick guide to use assembler in GCC have a look at https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html

```
Compile the application (make clean all)
Generate the assember (make dis > dis.s)
Run it in gui mode (make conf gui=1 run)
```

Identify the meaniful part in the assembler. Check both the trace (in the build folder)
and the waveforms of the core.

```
How many cycles do you exept from such function? Why?
```

## Loop Unrolling

Improve the application using the loop unrolling technique we have seen at the lecture.
Complete the provided function and repeat the step above.

```
How many cycles do you exept from such function?
Analyze the trace around your function. Where is the stall?
Introduce the c.nop instruction to align the address of the first instruction of the HWloop to get best performance.
```

## Use of the SIMD instructions

Rewrite the function to implement the dot product using the sum of dot product function
that you find the PULP extensions of the RI5CY core (in its user_manual.doc)

```
Complete the function.
How many cycles do you exept from such function?
Fix the HWloop first instruction address if needed.
What is the speed up?
```