#include <stdio.h>
#include "goc.h"

void __attribute__ ((constructor)) initLibrary(void)
{
  printf("libgoc is initialized\n");
}

void __attribute__ ((destructor)) cleanUpLibrary(void)
{
  printf("library is exited\n");
}

int Sum(int a, int b)
{
  return a + b;
}

int Extract(int a, int b)
{
  return a - b;
}

int Mul(int a, int b)
{
  return a * b;
}
