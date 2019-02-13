#include "pulp.h"

//#define ADD_NOP
//define ADD_NOP in case the HWLoop is unaligned, otherwise to fetch the first instruction it takes 2 cycles


unsigned int dotproduct(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N)
{
    int i;
    unsigned char elemA, elemB;
    unsigned int instr, cycles, ldstall, jrstall, imstall;

    rt_perf_t *perf;
    perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));

    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;

    rt_perf_reset(perf);

    //start the monitoring
    rt_perf_start(perf);

        asm volatile(
#ifdef ADD_NOP
            "c.nop;"
#endif
            "lp.setup x0,%[n_elem],stop_loop;"
            "p.lbu %[a], 1(%[addrA]!);"
            "p.lbu %[b], 1(%[addrB]!);"
            "stop_loop: p.mac %[c], %[a], %[b];"
            : [c] "+r" (acc), [a] "+r" (elemA), [b] "+r" (elemB), [addrA] "+r" (vA), [addrB] "+r" (vB) : [n_elem] "r" (N));

    //stop the HW counter used for monitoring
    rt_perf_stop(perf);

    //get the total measurement
    rt_perf_save(perf);

    instr   = rt_perf_get(perf, RT_PERF_INSTR);
    cycles  = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
    ldstall = rt_perf_get(perf, RT_PERF_LD_STALL);
    jrstall = rt_perf_get(perf, RT_PERF_JR_STALL);
    imstall = rt_perf_get(perf, RT_PERF_IMISS);

    printf("Perf of matrixAdd: \n \t cycles : %d \n \t instructions : %d \n \t load stalls : %d \n \t jump stalls : %d \n \t insrtructions stalls: %d \n\n", cycles, instr, ldstall, jrstall, imstall);


    return acc;
}

unsigned int dotproduct_loopunroll(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N)
{
    int i;
    unsigned char elemA, elemB, elemD, elemE;
    unsigned int instr, cycles, ldstall, jrstall, imstall;

    rt_perf_t *perf;
    perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));

    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;

    rt_perf_reset(perf);

    //start the monitoring
    rt_perf_start(perf);

        asm volatile(
#ifdef ADD_NOP
            "c.nop;"
#endif
            "lp.setup x0,%[n_elem],stop_loop_lu;"
            "p.lbu %[a], 1(%[addrA]!);"
            "p.lbu %[b], 1(%[addrB]!);"
            "p.lbu %[d], 1(%[addrA]!);"
            "p.lbu %[e], 1(%[addrB]!);"
            "p.mac %[c], %[a], %[b];"
            "stop_loop_lu: p.mac %[c], %[d], %[e];"
            : [c] "+r" (acc), [a] "+r" (elemA), [b] "+r" (elemB), [d] "+r" (elemD), [e] "+r" (elemD), [addrA] "+r" (vA), [addrB] "+r" (vB) : [n_elem] "r" (N>>1));

    //stop the HW counter used for monitoring
    rt_perf_stop(perf);

    //get the total measurement
    rt_perf_save(perf);

    instr   = rt_perf_get(perf, RT_PERF_INSTR);
    cycles  = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
    ldstall = rt_perf_get(perf, RT_PERF_LD_STALL);
    jrstall = rt_perf_get(perf, RT_PERF_JR_STALL);
    imstall = rt_perf_get(perf, RT_PERF_IMISS);

    printf("Perf of matrixAdd: \n \t cycles : %d \n \t instructions : %d \n \t load stalls : %d \n \t jump stalls : %d \n \t insrtructions stalls: %d \n\n", cycles, instr, ldstall, jrstall, imstall);


    return acc;
}


unsigned int dotproduct_loopunroll_simd(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N)
{
    int i;
    unsigned int elemA, elemB, elemD, elemE;
    unsigned int instr, cycles, ldstall, jrstall, imstall;

    rt_perf_t *perf;
    perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));

    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;

    rt_perf_reset(perf);

    //start the monitoring
    rt_perf_start(perf);

        asm volatile(
#ifdef ADD_NOP
            "c.nop;"
#endif
            "lp.setup x0,%[n_elem],stop_loop_simd;"
            "p.lw %[a], 4(%[addrA]!);"
            "p.lw %[b], 4(%[addrB]!);"
            "p.lw %[d], 4(%[addrA]!);"
            "p.lw %[e], 4(%[addrB]!);"
            "pv.sdotup.b %[c], %[a], %[b];"
            "stop_loop_simd: pv.sdotup.b %[c], %[d], %[e];"
            : [c] "+r" (acc), [a] "+r" (elemA), [b] "+r" (elemB), [d] "+r" (elemD), [e] "+r" (elemD), [addrA] "+r" (vA), [addrB] "+r" (vB) : [n_elem] "r" (N>>3));

    //stop the HW counter used for monitoring
    rt_perf_stop(perf);

    //get the total measurement
    rt_perf_save(perf);

    instr   = rt_perf_get(perf, RT_PERF_INSTR);
    cycles  = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
    ldstall = rt_perf_get(perf, RT_PERF_LD_STALL);
    jrstall = rt_perf_get(perf, RT_PERF_JR_STALL);
    imstall = rt_perf_get(perf, RT_PERF_IMISS);

    printf("Perf of matrixAdd: \n \t cycles : %d \n \t instructions : %d \n \t load stalls : %d \n \t jump stalls : %d \n \t insrtructions stalls: %d \n\n", cycles, instr, ldstall, jrstall, imstall);


    return acc;
}