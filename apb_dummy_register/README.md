# New APB Peripheral

Open the RTL folder of the pulp_soc IP.
This IP is the top level of architecture of the SoC.

```
sublime_text pulpissimo/ips/pulp_soc/rtl
```

## pulp_soc

pulp_soc instantiates the peripheral subsystem (soc_peripheral), the fabric controller (fc_subsystem)
the clock generator logic and the logaritmic interconnect (soc_interconnect_wrap), the boot rom and the
multi-bank memory subsystem (l2_ram_multi_bank).

The peripheral subsystem contains the APB bus (periph_bus_wrap) and all the APB peripherals including
the uDMA subsystem. The only APB peripherals missing here are the Interrupt controller and Debug unit and the HWCE.

The fabric controller contains the core with the debug unit, the interrupt controller and the HWCE.


## What to do

We want to add a new peripheral starting at `1A120000`.
Open the logaritmic interconnect bus.


```
What is the APB address range?
```

If it does not fit there, first thing to do is to expand the APB domain.

Once this is done, we need to extend the number of master ports in the peripheral bus and propagate
the changes to our new peripheral.


Connect all the signals you need to the `apb_dummy_registers` module and complete the RTL inside it.


```
Add a Read-Only  register at 1A120000 containing the world 0xDA41DE.
Add a Read-Write register at 1A120004 and initialize it with 0.
Complete the Read and Write logic.
Compile the application and simulate it in gui mode.
Open all the waveforms involved
- Core
- Log Interconnect
- Peripheral Bus
- apb_dummy_registers
Identify the momemnt you are actually reading and writing your peripheral and measure the latency from the Core request to the actual response.
```


