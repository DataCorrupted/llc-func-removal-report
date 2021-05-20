#include <stdio.h>
#include <stdlib.h>

#include "./dummy_func.h"

int main(int argc, char** argv) {
  int c = getc(stdin);
  int buf[255 + c];
  int d = dummy_func_explict();
  printf("%d\n", d);
  int b = getc(stdin);
  if (buf[b] > 42) {
    printf("Better than 42!");
  }
  return 0;
}