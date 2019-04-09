#include <stdio.h>
#include "cpu.h"

/**
 * Main
 */
// Change args to (int argc, char *argv[]) as per README
int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    struct cpu cpu;

    cpu_init(&cpu);
    // Change args for cpu_load
    // Because we changed this we also have to change args where the function is called in cpu.h and cpu.c
    cpu_load(&cpu, argv);
    cpu_run(&cpu);

    return 0;
  }
  printf("Invalid number of arguments entered. Please provide a file name \n");
  return 0;
}