#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// CPU UTILS
static void update_zero_and_negative_flags(cpu_t *cpu, u8 result);
static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am);

// OPCODES PROCEDURES
static void opcode_lda(cpu_t *cpu, int addr_mode);
static void opcode_tax(cpu_t *cpu, int addr_mode);
static void opcode_inx(cpu_t *cpu, int addr_mode);

static opcode_t optable[256];

static void opcode_assign(u8 code, char *mnemonic, int bytes, int cycles, opcode_proc proc, enum addressing_mode_t mode) {
	optable[code].code = code;
	strncpy(optable[code].mnemonic, mnemonic, 4);
	optable[code].bytes = bytes;
	optable[code].cycles = cycles;
	optable[code].proc = proc;
	optable[code].mode = mode;
}

static void generate_optable() {
	opcode_assign(0xAA, "TAX", 1, 2, opcode_tax, NONE);
	opcode_assign(0xE8, "INX", 1, 2, opcode_inx, NONE);

	opcode_assign(0xA9, "LDA", 2, 2, opcode_lda, IMM);
	opcode_assign(0xA5, "LDA", 2, 3, opcode_lda, ZP);
	opcode_assign(0xB5, "LDA", 2, 4, opcode_lda, ZPX);
	opcode_assign(0xAD, "LDA", 3, 4, opcode_lda, ABS);
	opcode_assign(0xBD, "LDA", 3, 4, opcode_lda, ABSX);
	opcode_assign(0xB9, "LDA", 3, 4, opcode_lda, ABSY);
	opcode_assign(0xA1, "LDA", 2, 6, opcode_lda, INDX);
	opcode_assign(0xB1, "LDA", 2, 5, opcode_lda, INDY);
}

cpu_t *cpu_init() {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));

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

	cpu_mem_write_u16(cpu, 0xFFFC, 0x8000);
}

void cpu_run(cpu_t *cpu) {
	while (true) {
		u8 code = cpu_mem_read(cpu, cpu->pc);
		cpu->pc += 1;

		opcode_t opcode = optable[code];

		switch (opscode) {
			// LDA
			case 0xA9: {
				u16 addr = get_operand_address(cpu, IMM);
				u8 param = cpu_mem_read(cpu, addr);

				cpu->a = param;

				update_zero_and_negative_flags(cpu, cpu->a);
				cpu->pc += 1;

			} break;

			case 0xA5: {
				u16 addr = get_operand_address(cpu, ZP);
				u8 param = cpu_mem_read(cpu, addr);

				cpu->a = param;

				update_zero_and_negative_flags(cpu, cpu->a);
				cpu->pc += 1;

			} break;

			case 0xAD: {
				u16 addr = get_operand_address(cpu, ABS);
				u8 param = cpu_mem_read(cpu, addr);

				cpu->a = param;

				update_zero_and_negative_flags(cpu, cpu->a);
				cpu->pc += 2;

			} break;

			// STA
			case 0x85: {
				u16 addr = get_operand_address(cpu, ZP);
				cpu_mem_write(cpu, addr, cpu->a);
				cpu->pc++;
			} break;

			case 0x95: {
				u16 addr = get_operand_address(cpu, ZPX);
				cpu_mem_write(cpu, addr, cpu->a);
				cpu->pc++;
			} break;

			// TAX
			case 0xAA: {
				cpu->x = cpu->a;		

				update_zero_and_negative_flags(cpu, cpu->x);
				
			} break;

			// INX
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
	printf("Register A: 0x%hhx\n", cpu->a);
	printf("Register X: 0x%hhx\n", cpu->x);
	printf("Register Y: 0x%hhx\n", cpu->y);
	printf("Status Register: 0x%hhx\n", cpu->sr);
}

void cpu_free(cpu_t *cpu) {
	free(cpu);
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

static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am) {
	switch (am) {
		case IMM:
			return cpu->pc;	
			break;

		case ZP:
			return (u16) cpu_mem_read(cpu, cpu->pc);
			break;
		case ZPX: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			return (u16) pos + cpu->x;
		} break;
		case ZPY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			return (u16) pos + cpu->y;
		} break;

		case ABS:
			return cpu_mem_read_u16(cpu, cpu->pc);
			break;
		case ABSX: {
			u16 pos = cpu_mem_read_u16(cpu, cpu->pc);
			return pos + (u16) cpu->x;
		} break;
		case ABSY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			return pos + (u16) cpu->y;
		} break;

		case INDX: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);		

			u8 ptr = pos + cpu->x;
			u8 lo = cpu_mem_read(cpu, (u16) ptr);
			u8 hi = cpu_mem_read(cpu, (u16) ptr + 1);
			return ((u16) hi) << 8 | ((u16) lo);
		} break;

		case INDY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);		

			u8 lo = cpu_mem_read(cpu, (u16) pos);
			u8 hi = cpu_mem_read(cpu, (u16) pos + 1);
			u16 deref_pos = ((u16) hi) << 8 | ((u16) lo);
			return deref_pos + (u16) cpu->y;

		} break;

		default: {
			printf("Addressing Mode not supported\n");
		} break;
	}

	return -1;
}

static void opcode_lda(cpu_t *cpu, int addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->a = value;
	update_zero_and_negative_flags(cpu, cpu->a);
}

static void opcode_tax(cpu_t *cpu, int addr_mode) {
	cpu->x = cpu->a;

	update_zero_and_negative_flags(cpu, cpu->x);
}

static void opcode_inx(cpu_t *cpu, int addr_mode) {
	cpu->x++;

	update_zero_and_negative_flags(cpu, cpu->x);
}
