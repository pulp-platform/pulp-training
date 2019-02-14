# Dot Product C program

This application contain a simple dot product function between 2 byte-vectors of 1000 elements.

```
Compile the application (make clean all)
Generate the assember (make dis > dis.s)
Run it in gui mode
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
Why don't you have your expectations met?
What can you do to solve it?
```

## Use of the SIMD instructions

Rewrite the function to implement the dot product using the sum of dot product function
that you find the PULP extensions of the RI5CY core (in its user_manual.doc)

```
Complete the function.
Repeat the step above.
```

Now open the udma_external_per_wrapper and see how it is organized.
Open the configuration registers (udma_external_per_reg_if.sv)

```
What is the address of the register REG_TX_CFG?
How many cycles do you exept from such function?
```