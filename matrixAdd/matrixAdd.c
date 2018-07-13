#include "pulp.h"

#if WORD == 32
#include "matrixAdd32.h"
#elif WORD == 16
#include "matrixAdd16.h"
#elif WORD == 8
#include "matrixAdd8.h"
#else
#error No word size specified
#endif

void matrixAdd(mattype * A, mattype * B, mattype * C, int N, int M);
void check_results(mattype *  C, int N, int M);
void check_matrix_add(testresult_t *result, void (*start)(), void (*stop)());

int main()
{
  int errors = 0;
  int N = matrixsizes;
  int M = matrixsizes;
  
  rt_perf_t *perf;
  perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  unsigned int instr, cycles, ldstall, jrstall, imstall;
  
  // load data to L1
  unsigned int i, j;
  for(i = 0; i < HEIGHT; i++) {
    for(j = 0; j < WIDTH; j++) {
      m_a[i*HEIGHT+j] = m_a_l2[i*HEIGHT+j];
      m_b[i*HEIGHT+j] = m_b_l2[i*HEIGHT+j];
    }
  }
  
  rt_perf_init(perf);
  rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;
  
  rt_perf_reset(perf);
  
  //start the monitoring
  rt_perf_start(perf);
  
  //execute the kernel
  matrixAdd(m_a, m_b, m_c, N, M);
  
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
  
  check_results(m_c, N, M);
  
}

void matrixAdd(mattype *  A, mattype *  B, mattype *  C, int N, int M)
{

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      C[i*N+j] = A[i*WIDTH+j] + B[i*WIDTH+j];
    }
  }
}

void check_results(mattype *  C, int N, int M)
{
  // check
  int i, j;
  for(i = 0; i < N; i++) {
    for(j = 0; j < M; j++) {
      if(C[i*N+j] != m_exp_l2[i*WIDTH+j]) {
#ifdef DEBUG_OUTPUT
	printf("Error at index %d, %d, expected %d, got %d\n", i, j, m_exp_l2[i*WIDTH+j], C[i*N+j]);
#endif
      }
    }
  }
}
