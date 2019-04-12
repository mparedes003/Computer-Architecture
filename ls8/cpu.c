#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6
// The Stack Pointer
#define SP 7

unsigned int ram_address = 0;

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
// void cpu_load(struct cpu *cpu)
void cpu_load(struct cpu *cpu, char *argv[])
{
  // char data[DATA_LEN] = {
  //     // From print8.ls8
  //     0b10000010, // LDI R0,8
  //     0b00000000,
  //     0b00001000,
  //     0b01000111, // PRN R0
  //     0b00000000,
  //     0b00000001 // HLT
  // };

  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++)
  // {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded
  FILE *fp;
  // open file for reading
  fp = fopen(argv[1], "r");
  char line[1024];

  // If file doesn't open or doesn't exist
  if (fp == NULL)
  {
    fprintf(stderr, "File %s cannot be opened or does not exist. \n", argv[1]);
    exit(1);
  }

  // fgets returns line
  // Chars are read from file fp and stored in line
  // Max num of chars read == sizeof(line)
  while (fgets(line, sizeof(line), fp) != NULL)
  {
    // *endptr
    char *converted_line;

    // Converts line from binary string to integer value
    // Unsigned long int strtoul(const char *str, char **endptr, int base)
    unsigned char byte = strtoul(line, &converted_line, 2);

    if (converted_line == line)
    {
      // fprintf("Skipping: %s", line);
      continue;
    }

    cpu->ram[ram_address++] = byte;
  }

  fclose(fp);
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
  case ALU_ADD:
    cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
    break;

  default:
    break;
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

    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      cpu->PC += shift;
      break;

    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      cpu->PC += shift;
      break;

    case POP:
      cpu->registers[operandA] = cpu_ram_read(cpu, cpu->registers[SP]++);
      if (cpu->registers[SP] > 255)
      {
        cpu->registers[SP] = 0xF4;
      }
      cpu->PC += shift;
      break;

    case PUSH:
      if (--cpu->registers[SP] <= ram_address)
      {
        fprintf(stderr, "Warning: Stack overflow.\n");
        exit(1);
      }
      cpu_ram_write(cpu, cpu->registers[SP], cpu->registers[operandA]);
      cpu->PC += shift;
      break;

    case CALL:
      // Decrement the value stored at registers[SP] by 2
      cpu->registers[SP]--;
      cpu_ram_write(cpu, cpu->registers[SP], cpu->PC + 2);
      cpu->PC = cpu->registers[operandA];
      break;

    case RET:
      cpu->PC = cpu_ram_read(cpu, cpu->registers[SP]);
      cpu->registers[SP]++;
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

  // The stack pointer is always stored in the register at reg[SP]
  // upon init, its value is set to number 0xF4 which corresponds to an index in ram[]
  // the stack should start at the top of memory at a high address and get larger downward as things are pushed on
  cpu->registers[SP] = 0xF4;
}
