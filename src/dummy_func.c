#include "stdio.h"

char __attribute__((noinline)) dummy_func_icmp() {
  printf("dummy_func called before icmp inst.\n");
  return 1;
}

char __attribute__((noinline)) dummy_func_getc() {
  printf("dummy_func called before _IO_getc().\n");
  return 1;
}