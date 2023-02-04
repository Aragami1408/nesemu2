#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// CPU UTILS
static void set_flag(cpu_t *cpu, bool v, u8 flag);
static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am);

static opcode_t optable[0xFF + 1];


static void optable_assign(u8 code, char *mnemonic, int bytes, int cycles, opcode_func func, enum addressing_mode_t mode) {
	optable[code].code = code;
	strcpy(optable[code].mnemonic, mnemonic);
	optable[code].bytes = bytes;
	optable[code].cycles = cycles;
	optable[code].func = func;
	optable[code].mode = mode;
}

static void optable_generator() {
	optable_assign(0x00, "BRK", 2, 2, NULL, NONE); // Don't worry. opcode_func of BRK is already handled in cpu_run's loop already
												   //
	optable_assign(0x29, "AND", 2, 2, &opcode_and, IMM);
	optable_assign(0x25, "AND", 2, 3, &opcode_and, ZP);
	optable_assign(0x35, "AND", 2, 4, &opcode_and, ZPX);
	optable_assign(0x2d, "AND", 3, 4, &opcode_and, ABS);
	optable_assign(0x3d, "AND", 3, 4, &opcode_and, ABSX);
	optable_assign(0x39, "AND", 3, 4, &opcode_and, ABSY);
	optable_assign(0x21, "AND", 2, 6, &opcode_and, INDX);
	optable_assign(0x31, "AND", 2, 5, &opcode_and, INDY);

	optable_assign(0xca, "DEX", 1, 2, &opcode_dex, NONE);
	optable_assign(0x88, "DEY", 1, 2, &opcode_dey, NONE);

	optable_assign(0xaa, "TAX", 1, 2, &opcode_tax, NONE);	
	optable_assign(0xa8, "TAY", 1, 2, &opcode_tay, NONE);	
	optable_assign(0x8a, "TXA", 1, 2, &opcode_txa, NONE);	
	optable_assign(0x98, "TYA", 1, 2, &opcode_tya, NONE);	
	optable_assign(0xba, "TSX", 1, 2, &opcode_tsx, NONE);
	optable_assign(0x9a, "TXS", 1, 2, &opcode_txs, NONE);

	optable_assign(0xe8, "INX", 1, 2, &opcode_inx, NONE);
	optable_assign(0xc8, "INY", 1, 2, &opcode_iny, NONE);

	optable_assign(0xa9, "LDA", 2, 2, &opcode_lda, IMM);
	optable_assign(0xa5, "LDA", 2, 3, &opcode_lda, ZP);
	optable_assign(0xb5, "LDA", 2, 4, &opcode_lda, ZPX);
	optable_assign(0xad, "LDA", 3, 4, &opcode_lda, ABS);
	optable_assign(0xbd, "LDA", 3, 4, &opcode_lda, ABSX);
	optable_assign(0xb9, "LDA", 3, 4, &opcode_lda, ABSY);
	optable_assign(0xa1, "LDA", 2, 6, &opcode_lda, INDX);
	optable_assign(0xb1, "LDA", 2, 5, &opcode_lda, INDY);
	
	optable_assign(0xa2, "LDX", 2, 2, &opcode_ldx, IMM);
	optable_assign(0xa6, "LDX", 2, 3, &opcode_ldx, ZP);
	optable_assign(0xb6, "LDX", 2, 4, &opcode_ldx, ZPY);
	optable_assign(0xae, "LDX", 3, 4, &opcode_ldx, ABS);
	optable_assign(0xbe, "LDX", 3, 4, &opcode_ldx, ABSY);

	optable_assign(0xa0, "LDY", 2, 2, &opcode_ldx, IMM);
	optable_assign(0xa4, "LDY", 2, 3, &opcode_ldx, ZP);
	optable_assign(0xb4, "LDY", 2, 4, &opcode_ldx, ZPX);
	optable_assign(0xac, "LDY", 3, 4, &opcode_ldx, ABS);
	optable_assign(0xbc, "LDY", 3, 4, &opcode_ldx, ABSY);
	
	optable_assign(0x85, "STA", 2, 3, &opcode_sta, ZP);
	optable_assign(0x95, "STA", 2, 4, &opcode_sta, ZPX);
	optable_assign(0x8d, "STA", 3, 4, &opcode_sta, ABS);
	optable_assign(0x9d, "STA", 3, 5, &opcode_sta, ABSX);
	optable_assign(0x99, "STA", 3, 5, &opcode_sta, ABSY);
	optable_assign(0x81, "STA", 2, 6, &opcode_sta, INDX);
	optable_assign(0x91, "STA", 2, 6, &opcode_sta, INDY);

	optable_assign(0x86, "STX", 2, 3, &opcode_stx, ZP);
	optable_assign(0x96, "STX", 2, 4, &opcode_stx, ZPY);
	optable_assign(0x8e, "STX", 3, 4, &opcode_stx, ABS);

	optable_assign(0x84, "STY", 2, 3, &opcode_sty, ZP);
	optable_assign(0x94, "STY", 2, 4, &opcode_sty, ZPX);
	optable_assign(0x8c, "STY", 3, 4, &opcode_sty, ABS);

}

cpu_t *cpu_init() {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));
	if (cpu == NULL) {
		LOG_ERROR("Cannot initialize CPU\n");
		return NULL;
	}

	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;

	cpu->pc = 0;
	cpu->sp = 0;
	cpu->sr = 0;

	optable_generator();

	LOG_INFO("CPU initialized successfully!");

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
		u16 pc_state = cpu->pc;

		opcode_t opcode = optable[code];
		if (code == 0x00) break;
		else if (opcode.func == NULL) {
			LOG_ERROR("Unknown opcode: 0x%hhx\n", opcode);
			break;
		}
		else {
			opcode.func(cpu, opcode.mode);
		}
		

		if (pc_state == cpu->pc) cpu->pc += (u16) (opcode.bytes - 1);
	}
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

static void set_flag(cpu_t *cpu, bool v, u8 flag) {
	if (v) cpu->sr |= flag;
	else cpu->sr &= ~flag;
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

void opcode_and(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	cpu->a &= value;
}

void opcode_dex(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x--;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, cpu->x & 0x80, SF_NEGATIVE);
}

void opcode_dey(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y--;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);
}

void opcode_lda(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->a = value;
	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, cpu->a & 0x80, SF_NEGATIVE);
}

void opcode_ldx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->x = value;
	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, cpu->x & 0x80, SF_NEGATIVE);
}

void opcode_ldy(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->y = value;
	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);
}

void opcode_tax(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x = cpu->a;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, cpu->x & 0x80, SF_NEGATIVE);
}

void opcode_tay(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y = cpu->a;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);
}

void opcode_txa(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->a = cpu->x;

	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, cpu->a & 0x80, SF_NEGATIVE);
}

void opcode_tya(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y = cpu->a;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);
}

void opcode_tsx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x = cpu->sp;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, cpu->x & 0x80, SF_NEGATIVE);
}

void opcode_txs(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sp = cpu->x;
}

void opcode_inx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x++;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, cpu->x & 0x80, SF_NEGATIVE);
}

void opcode_iny(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y++;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);

}

void opcode_sta(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->a);
}

void opcode_stx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->x);
	
}

void opcode_sty(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->y);

}
