#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void update_zero_and_negative_flags(cpu_t *cpu, u8 result);

cpu_t *cpu_init() {
	cpu_t *cpu = malloc(sizeof(cpu_t));

	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;

	cpu->pc = 0;
	cpu->sp = 0;
	cpu->sr = 0;

	return cpu;
}

u8 cpu_mem_read(cpu_t *cpu, u16 addr) {
	return cpu->memory[addr];
}

void cpu_mem_write(cpu_t *cpu, u16 addr, u8 data) {
	cpu->memory[addr] = data;
}

u16 cpu_mem_read_u16(cpu_t *cpu, u16 pos) {
	u16 lo = cpu_mem_read(cpu, pos);
	u16 hi = cpu_mem_read(cpu, pos + 1);

	return (hi << 8) | (lo);
}

void cpu_mem_write_u16(cpu_t *cpu, u16 pos, u16 data) {
	u8 hi = data >> 8;
	u8 lo = data & 0xff;
	cpu_mem_write(cpu, pos, lo);
	cpu_mem_write(cpu, pos + 1, hi);
}

void cpu_reset(cpu_t *cpu) {
	cpu->a = 0;
	cpu->x = 0;
	cpu->sr = 0;

	cpu->pc = cpu_mem_read_u16(cpu, 0xFFFC);
}

void cpu_load(cpu_t *cpu, u8 *program, int size) {
	for (int i = 0; i < size; i++) {
		cpu->memory[0x8000 + i] = program[i];
	}

	cpu->pc = 0x8000;
}

void cpu_run(cpu_t *cpu) {
	cpu->pc = 0;

	while (true) {
		u8 opscode = cpu_mem_read(cpu, cpu->pc);
		cpu->pc += 1;

		switch (opscode) {
			case 0xA9: {
				u8 param = cpu_mem_read(cpu, cpu->pc);
				cpu->pc += 1;
				cpu->a = param;

				update_zero_and_negative_flags(cpu, cpu->a);

			} break;
			case 0xAA: {
				cpu->x = cpu->a;		

				update_zero_and_negative_flags(cpu, cpu->x);
				
			} break;

			case 0xE8: {
				cpu->x++;

				update_zero_and_negative_flags(cpu, cpu->x);
			} break;
			case 0x00: 
				goto eof;
				break;
			default: {
				printf("Unknown opcode");	
				goto eof;
			} break;
		}
	}

eof: ;
}

void cpu_load_and_run(cpu_t *cpu, u8 *program, int size) {
	cpu_load(cpu, program, size);
	cpu_reset(cpu);
	cpu_run(cpu);
}

void cpu_regdump(cpu_t *cpu) {
	printf("Register A: %hhu\n", cpu->a);
	printf("Register X: %hhu\n", cpu->x);
	printf("Register Y: %hhu\n", cpu->y);
	printf("Status Register: %hhu\n", cpu->sr);
}

static void update_zero_and_negative_flags(cpu_t *cpu, u8 result) {
	// Zero flag: 0b0000_0010
	if (result == 0) {
		cpu->sr |= 0x2; 
	}
	else {
		cpu->sr &= 0xFD;
	}

	// Negative flag: 0b1000_0000
	if ((result & 0x80) != 0) {
		cpu->sr |= 0x80;
	}
	else {
		cpu->sr &= 0x7F;
	}
}
