#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

// Add functions cpu_ram_read() and cpu_ram_write()
// that access the RAM inside the struct cpu

// Read function reads from ram, it returns an unsigned char,
// based on how it is defined in struct cpu:

// Accepts 2 arguments
// A pointer to the cpu and an index in ram[] to access
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{

  // Return the data held in ram at the specified index
  return cpu->ram[index];
}

// Write function no need to return anything specific
// It can have the type of void

// Accepts 3 arguments
// A pointer to the cpu, an index to be written to, and a value to write
void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char value)
{

  // Assign the specified value to the value at specified index
  cpu->ram[index] = value;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu)
{
  char data[DATA_LEN] = {
      // From print8.ls8
      0b10000010, // LDI R0,8
      0b00000000,
      0b00001000,
      0b01000111, // PRN R0
      0b00000000,
      0b00000001 // HLT
  };

  int address = 0;

  for (int i = 0; i < DATA_LEN; i++)
  {
    cpu->ram[address++] = data[i];
  }

  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).

    // Reads the memory address that is stored in register PC
    // and stores that result in current_instruction == IR, the Instruction Register
    unsigned char current_instruction = cpu_ram_read(cpu, cpu->PC);

    // 2. Figure out how many operands this next instruction requires
    unsigned int num_operands = current_instruction >> 6;

    // 3. Get the appropriate value(s) of the operands following this instruction

    // Uses cpu_ram_read(), to read the bytes at cpu->PC+1 and cpu->PC+2 from RAM
    // into variables operandA and operandB in case the instruction needs them
    unsigned operandA = cpu_ram_read(cpu, (cpu->PC + 1));
    unsigned operandB = cpu_ram_read(cpu, (cpu->PC + 2));

    // 6. Move the PC to the next instruction.
    int shift = (num_operands) + 1;

    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    switch (current_instruction)
    {

    // LDI
    // Sets a specified register to a specified value
    // Takes 2 arguments: 'register' and 'immediate'
    // Sets the next instruction to the instruction 2 steps ahead
    case LDI:
      cpu->registers[operandA] = operandB;
      cpu->PC += shift;
      break;

    // PRN
    // Prints a numeric value stored in the given register
    case PRN:
      printf("%d \n", cpu->registers[operandA]);
      cpu->PC += shift;
      break;

    // HLT
    // Halts/Stops the CPU and exits the emulator
    case HLT:
      running = 0; // Stops the 'while (running)' loop
      break;

    default:
      break;
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  // Assign 0X00 to the value of PC in struct cpu
  cpu->PC = 0x00;

  // Use memset to fill a block of memory
  // in registers, with a value initially set to 0, in a block of memory that is the size of the registers inside struct cpu
  memset(cpu->registers, 0, sizeof(cpu->registers));
  // Use memset to fill a block of memory
  // in ram, with a value initially set 0, in a block of memory that is the size of the ram inside struct cpu
  memset(cpu->ram, 0, sizeof(cpu->ram));
}
