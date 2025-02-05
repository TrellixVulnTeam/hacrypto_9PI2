/*
cpucycles/cortex.c version 20180222
D. J. Bernstein
Public domain.
*/

/*
 * To use this, your kernel must enable reading the cycle counter
 * from userland code.
 *
 * Sample code for Armv7 (32 bits):
 * <https://github.com/thoughtpolice/enable_arm_pmu>
 *
 * Sample code for Armv8/Aarch64 (64 bits):
 * <https://github.com/rdolbeau/enable_arm_pmu>
 */

#define SCALE 1
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "cpucycles.h"

static int enabled = 0;

static int prev[3];
static unsigned long long prevcycles = 0;
static int now[3];
static long long cyclespersec = 0;

#if defined(__aarch64__)
long long cpucycles(void)
{
  long long Rt;
  asm volatile("mrs %0, PMCCNTR_EL0" : "=r" (Rt));
  return Rt;
}
long long cpucycles_persecond(void) {
  struct timeval t0,t1;
  long long c0,c1;
  double d0,d1;
  gettimeofday(&t0,(struct timezone *) 0);
  c0 = cpucycles();
  sleep(1);
  gettimeofday(&t1,(struct timezone *) 0);
  c1 = cpucycles();
  d0 = (double) t0.tv_sec;
  d0 += ((double) t0.tv_usec) / 1000000.0;
  d1 = (double) t1.tv_sec;
  d1 += ((double) t1.tv_usec) / 1000000.0;
  return  (c1-c0)/(d1-d0);
}
#else
static void readticks(int *result)
{
  struct timeval t;
  unsigned int cc;
  if (!enabled) {
    asm volatile("mcr p15, 0, %0, c9, c12, 0" :: "r"(17));
    asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x8000000f));
    asm volatile("mcr p15, 0, %0, c9, c12, 3" :: "r"(0x8000000f));
    enabled = 1;
  }
  asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(cc));
  gettimeofday(&t,(struct timezone *) 0);
  result[0] = cc;
  result[1] = t.tv_usec;
  result[2] = t.tv_sec;
}

long long cpucycles(void)
{
  unsigned long long delta4;
  int deltan;
  int deltas;
  unsigned long long guesscycles;
  
  readticks(now);
  delta4 = (unsigned int) (now[0] - prev[0]); /* unsigned change in number of cycles mod 2^32 */
  deltan = now[1] - prev[1]; /* signed change in number of nanoseconds mod 10^9 */
  deltas = now[2] - prev[2]; /* signed change in number of seconds */
  if ((deltas == 0 && deltan < 200000) || (deltas == 1 && deltan < -800000))
    return (prevcycles + delta4) * SCALE;

  prev[0] = now[0];
  prev[1] = now[1];
  prev[2] = now[2];

  if ((deltas == 0 && deltan < 300000) || (deltas == 1 && deltan < -700000)) {
    // actual number of cycles cannot have increased by 2^32 in <0.3ms
    cyclespersec = 1000000 * (unsigned long long) delta4;
    cyclespersec /= deltan + 1000000 * (long long) deltas;
  } else {
    guesscycles = deltas * cyclespersec;
    guesscycles += (deltan * cyclespersec) / 1000000;
    while (delta4 + 2147483648ULL < guesscycles) delta4 += 4294967296ULL;
    /* XXX: could do longer-term extrapolation here */
  }

  prevcycles += delta4;
  return prevcycles * SCALE;
}

long long cpucycles_persecond(void)
{
  while (!cyclespersec) cpucycles();
  return cyclespersec * SCALE;
}
#endif
