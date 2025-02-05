#include <sys/types.h>
#include <sys/sysctl.h>
#include "cpucycles.h"

long long cpucycles(void)
{
  long long result;
  asm volatile(".byte 15;.byte 49" : "=A" (result));
  return result;
}

long long cpucycles_persecond(void)
{
  long result = 0;
  size_t resultlen = sizeof(long);
  sysctlbyname("machdep.tsc_freq",&result,&resultlen,0,0);
  return result;
}
