# New uDMA Peripheral

Open the uDMA subsystem RTL file and the uDMA ip folder.

```
sublime_text pulpissimo/ips/udma
sublime_text pulpissimo/ips/pulp_soc/rtl/udma_subsystem/
```

## udma_subsystem

udma_subsystem is instantiated in the peripheral subsystem (soc_peripheral) and contains
all the peripherals needed by the chip, it defines the IDs of those and connect them to the
udma_core.
It also generates events coming from peripherals.


## What to do

Look how the uDMA is organized and identify the concepts explain during the theoretical class.

```
What do all the peripherals have in common?
What is the commond interface between them and the udma_core?
```

We are going to add a new peripheral (udma_external_per_wrapper) to the uDMA ad ID 8.
The uDMA is mapped to `0x1A102000`.
The first peripheral (ID = 0) is mapped to `0x1A102080` and so and so on.

```
Please complete the RTL to connect it to the udma_core.
What is the address of the new peripheral (ID=8) ?
```

Now open the udma_external_per_wrapper and see how it is organized.
Open the configuration registers (udma_external_per_reg_if.sv)

```
What is the address of the register REG_TX_CFG?
```

The modules `udma_traffic_gen_rx` and `udma_traffic_gen_tx` implement the peripheral protocol.
In this example, this peripheral (traffic generator) is not connected to the external world (no PIN outside)
and it does not implement any transmission protocol.
The reception protocol (rx) in this case is just a sequence of writes to the memory starting from a programmed
value up to that value + 256.

We want the initial value being mapped in the last 16bits of the REG_EXTERNAL_PER_SETUP register and the
number of data generated in the second byte of the same register.

```
Complete the RTL to implement the specification above.
```

Every peripheral communicates with the udma_core via the `valid, ready, data` protocol we have seen in the slides (RX Channel).


```
Complete the FSM of the traffic generator to generate when there is a new valid data to send to the udma_core.
Remember, every new data generates a new valid and before sending the next data it should wait for the ready.
```

```
As you may have noticed from the udma_traffic_gen_rx FSM, the peripheral starts when the programmer writes 1
to the bits 0 of the REG_EXTERNAL_PER_SETUP register. Once the transfer is done, the FSM should go to WAIT_CLEAR and wait
the programmer to write 0 again to the same bit to go back to IDLE.
```


Now open the C program. First we enable the CLOCK in the udma_core then we need to set the starting address of our
buffer in memory in REG_RX_SADDR, the number of bytes to transfer in REG_RX_SIZE and set the initial value (can be 0)
and the number of elements.


```
Complete the C program to add the missing instructions.
Look at the RTL peripheral, how does the programmer now when the transfer is finished?
Implement a mechanism to wait the transfer to be done.
Compile the application and simulate it in gui mode.
Open all the waveforms involved
- Core
- uDMA Core and submodules you may need
- udma_external_per_wrapper and submodules
Identify the momemnt you are actually reading and writing your peripheral and measure the
bandwidth from the peripheral to the memory.
```


