#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "osfreq.c"
#include "cpucycles.h"

long long cpucycles(void)
{
  unsigned long long result;
  asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");
  return result;
}

long long cpucycles_persecond(void)
{
  return osfreq();
}
