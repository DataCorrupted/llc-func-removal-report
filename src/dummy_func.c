#include "stdio.h"

static int global = 0;

char __attribute__((noinline)) dummy_func_icmp() {
  printf("dummy_func called before icmp inst.\n");
  return global++;
}

char __attribute__((noinline)) dummy_func_getc() {
  printf("dummy_func called before _IO_getc().\n");
  return global++;
}

int __attribute__((noinline)) dummy_func_explict() {
  printf("dummy_func_explict called.\n");
  return global++;
}