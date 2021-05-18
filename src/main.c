#include "stdio.h"
#include "stdlib.h"

int main(int argc, char** argv) {
  int c = getc(stdin);
  int buf[255 + c];
  int b = getc(stdin);
  if (buf[b] > 42) {
    printf("Better than 42!");
  }
  return 0;
}