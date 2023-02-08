#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// CPU UTILS

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
	optable_assign(0x00, "BRK", 1, 7, NULL, NONE); // Don't worry. opcode_func of BRK is already handled in cpu_run's loop already
	optable_assign(0xea, "NOP", 1, 2, NULL, NONE);

	optable_assign(0x69, "ADC", 2, 2, &opcode_adc, IMM);
	optable_assign(0x65, "ADC", 2, 3, &opcode_adc, ZP);
	optable_assign(0x75, "ADC", 2, 4, &opcode_adc, ZPX);
	optable_assign(0x6d, "ADC", 3, 4, &opcode_adc, ABS);
	optable_assign(0x7d, "ADC", 3, 4, &opcode_adc, ABSX);
	optable_assign(0x79, "ADC", 3, 4, &opcode_adc, ABSY);
	optable_assign(0x61, "ADC", 2, 6, &opcode_adc, INDX);
	optable_assign(0x71, "ADC", 2, 5, &opcode_adc, INDY);

	optable_assign(0xe9, "SBC", 2, 2, &opcode_sbc, IMM);
	optable_assign(0xe5, "SBC", 2, 3, &opcode_sbc, ZP);
	optable_assign(0xf5, "SBC", 2, 4, &opcode_sbc, ZPX);
	optable_assign(0xed, "SBC", 3, 4, &opcode_sbc, ABS);
	optable_assign(0xfd, "SBC", 3, 4, &opcode_sbc, ABSX);
	optable_assign(0xf9, "SBC", 3, 4, &opcode_sbc, ABSY);
	optable_assign(0xe1, "SBC", 2, 6, &opcode_sbc, INDX);
	optable_assign(0xf1, "SBC", 2, 5, &opcode_sbc, INDY);

	optable_assign(0xc9, "CMP", 2, 2, &opcode_cmp, IMM);
	optable_assign(0xc5, "CMP", 2, 3, &opcode_cmp, ZP);
	optable_assign(0xd5, "CMP", 2, 4, &opcode_cmp, ZPX);
	optable_assign(0xcd, "CMP", 3, 4, &opcode_cmp, ABS);
	optable_assign(0xdd, "CMP", 3, 4, &opcode_cmp, ABSX);
	optable_assign(0xd9, "CMP", 3, 4, &opcode_cmp, ABSY);
	optable_assign(0xc1, "CMP", 2, 6, &opcode_cmp, INDX);
	optable_assign(0xd1, "CMP", 2, 5, &opcode_cmp, INDY);

	optable_assign(0xe0, "CPX", 2, 2, &opcode_cpx, IMM);
	optable_assign(0xe4, "CPX", 2, 3, &opcode_cpx, ZP);
	optable_assign(0xec, "CPX", 3, 4, &opcode_cpx, ABS);

	optable_assign(0xc0, "CPY", 2, 2, &opcode_cpy, IMM);
	optable_assign(0xc4, "CPY", 2, 3, &opcode_cpy, ZP);
	optable_assign(0xcc, "CPY", 3, 4, &opcode_cpy, ABS);

	
	optable_assign(0x29, "AND", 2, 2, &opcode_and, IMM);
	optable_assign(0x25, "AND", 2, 3, &opcode_and, ZP);
	optable_assign(0x35, "AND", 2, 4, &opcode_and, ZPX);
	optable_assign(0x2d, "AND", 3, 4, &opcode_and, ABS);
	optable_assign(0x3d, "AND", 3, 4, &opcode_and, ABSX);
	optable_assign(0x39, "AND", 3, 4, &opcode_and, ABSY);
	optable_assign(0x21, "AND", 2, 6, &opcode_and, INDX);
	optable_assign(0x31, "AND", 2, 5, &opcode_and, INDY);

	optable_assign(0x49, "EOR", 2, 2, &opcode_eor, IMM);
	optable_assign(0x45, "EOR", 2, 3, &opcode_eor, ZP);
	optable_assign(0x55, "EOR", 2, 4, &opcode_eor, ZPX);
	optable_assign(0x4d, "EOR", 3, 4, &opcode_eor, ABS);
	optable_assign(0x5d, "EOR", 3, 4, &opcode_eor, ABSX);
	optable_assign(0x59, "EOR", 3, 4, &opcode_eor, ABSY);
	optable_assign(0x41, "EOR", 2, 6, &opcode_eor, INDX);
	optable_assign(0x51, "EOR", 2, 5, &opcode_eor, ABSY);

	optable_assign(0x09, "ORA", 2, 2, &opcode_ora, IMM);
	optable_assign(0x05, "ORA", 2, 3, &opcode_ora, ZP);
	optable_assign(0x15, "ORA", 2, 4, &opcode_ora, ZPX);
	optable_assign(0x0d, "ORA", 3, 4, &opcode_ora, ABS);
	optable_assign(0x1d, "ORA", 3, 4, &opcode_ora, ABSX);
	optable_assign(0x19, "ORA", 3, 4, &opcode_ora, ABSY);
	optable_assign(0x01, "ORA", 2, 6, &opcode_ora, INDX);
	optable_assign(0x11, "ORA", 2, 5, &opcode_ora, INDY);

	optable_assign(0x24, "BIT", 2, 3, &opcode_bit, ZP);
	optable_assign(0x2c, "BIT", 3, 4, &opcode_bit, ABS);



	optable_assign(0xe6, "INC", 2, 5, &opcode_inc, ZP);
	optable_assign(0xf6, "INC", 2, 6, &opcode_inc, ZPX);
	optable_assign(0xee, "INC", 3, 6, &opcode_inc, ABS);
	optable_assign(0xfe, "INC", 3, 7, &opcode_inc, ABSX);

	optable_assign(0xe8, "INX", 1, 2, &opcode_inx, NONE);
	optable_assign(0xc8, "INY", 1, 2, &opcode_iny, NONE);

	optable_assign(0xc6, "DEC", 2, 5, &opcode_dec, ZP);
	optable_assign(0xd6, "DEC", 2, 6, &opcode_dec, ZPX);
	optable_assign(0xce, "DEC", 3, 6, &opcode_dec, ABS);
	optable_assign(0xde, "DEC", 3, 7, &opcode_dec, ABSX);

	optable_assign(0xca, "DEX", 1, 2, &opcode_dex, NONE);
	optable_assign(0x88, "DEY", 1, 2, &opcode_dey, NONE);

	optable_assign(0x0a, "ASL", 1, 2, &opcode_asl, ACC);
	optable_assign(0x06, "ASL", 2, 5, &opcode_asl, ZP);
	optable_assign(0x16, "ASL", 2, 6, &opcode_asl, ZPX);
	optable_assign(0x0e, "ASL", 3, 6, &opcode_asl, ABS);
	optable_assign(0x1e, "ASL", 3, 7, &opcode_asl, ABSX);
	optable_assign(0x4a, "LSR", 1, 2, &opcode_lsr, ACC);
	optable_assign(0x46, "LSR", 2, 5, &opcode_lsr, ZP);
	optable_assign(0x56, "LSR", 2, 6, &opcode_lsr, ZPX);
	optable_assign(0x4e, "LSR", 3, 6, &opcode_lsr, ABS);
	optable_assign(0x5e, "LSR", 3, 7, &opcode_lsr, ABSX);
	optable_assign(0x2a, "ROL", 1, 2, &opcode_rol, ACC);
	optable_assign(0x26, "ROL", 2, 5, &opcode_lsr, ZP);
	optable_assign(0x36, "ROL", 2, 6, &opcode_lsr, ZPX);
	optable_assign(0x2e, "ROL", 3, 6, &opcode_lsr, ABS);
	optable_assign(0x3e, "ROL", 3, 7, &opcode_lsr, ABSX);

	optable_assign(0xaa, "TAX", 1, 2, &opcode_tax, NONE);	
	optable_assign(0xa8, "TAY", 1, 2, &opcode_tay, NONE);	
	optable_assign(0x8a, "TXA", 1, 2, &opcode_txa, NONE);	
	optable_assign(0x98, "TYA", 1, 2, &opcode_tya, NONE);	

	optable_assign(0xba, "TSX", 1, 2, &opcode_tsx, NONE);
	optable_assign(0x9a, "TXS", 1, 2, &opcode_txs, NONE);
	optable_assign(0x48, "PHA", 1, 3, &opcode_pha, NONE);
	optable_assign(0x08, "PHP", 1, 3, &opcode_php, NONE);
	optable_assign(0x68, "PLA", 1, 4, &opcode_pla, NONE);
	optable_assign(0x28, "PLP", 1, 4, &opcode_plp, NONE);

	optable_assign(0x4c, "JMP", 3, 3, &opcode_jmp, ABS);
	optable_assign(0x6c, "JMP", 3, 5, &opcode_jmp, IND);

	optable_assign(0x20, "JSR", 3, 6, &opcode_jsr, ABS);
	optable_assign(0x60, "RTS", 1, 6, &opcode_rts, NONE);

	optable_assign(0x90, "BCC", 2, 2, &opcode_bcc, NONE);
	optable_assign(0xb0, "BCS", 2, 2, &opcode_bcs, NONE);
	optable_assign(0xf0, "BEQ", 2, 2, &opcode_beq, NONE);
	optable_assign(0x30, "BMI", 2, 2, &opcode_bmi, NONE);
	optable_assign(0xd0, "BNE", 2, 2, &opcode_bne, NONE);
	optable_assign(0x10, "BPL", 2, 2, &opcode_bpl, NONE);
	optable_assign(0x50, "BVC", 2, 2, &opcode_bvc, NONE);
	optable_assign(0x70, "BVS", 2, 2, &opcode_bvs, NONE);

	optable_assign(0x18, "CLC", 1, 2, &opcode_clc, NONE);
	optable_assign(0xd8, "CLD", 1, 2, &opcode_cld, NONE);
	optable_assign(0x58, "CLI", 1, 2, &opcode_cli, NONE);
	optable_assign(0xb8, "CLV", 1, 2, &opcode_clv, NONE);
	optable_assign(0x38, "SEC", 1, 2, &opcode_sec, NONE);
	optable_assign(0xf8, "SED", 1, 2, &opcode_sed, NONE);
	optable_assign(0x78, "SEI", 1, 2, &opcode_sei, NONE);


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
	optable_assign(0xa0, "LDY", 2, 2, &opcode_ldy, IMM);
	optable_assign(0xa4, "LDY", 2, 3, &opcode_ldy, ZP);
	optable_assign(0xb4, "LDY", 2, 4, &opcode_ldy, ZPX);
	optable_assign(0xac, "LDY", 3, 4, &opcode_ldy, ABS);
	optable_assign(0xbc, "LDY", 3, 4, &opcode_ldy, ABSY);
	
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
	cpu->sp = STACK_RESET;
	cpu->sr = 0b100100;

	for (u16 i = 0; i < MEMORY_MAX; i++) cpu->memory[i] = 0;
	

	optable_generator();

	LOG_INFO("CPU initialized successfully!\n");

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

void cpu_stack_push(cpu_t *cpu, u8 data) {
	cpu_mem_write(cpu, (u16)(STACK + cpu->sp), data);
	cpu->sp -= 1;
}

u8 cpu_stack_pop(cpu_t *cpu) {
	cpu->sp++;
	return cpu_mem_read(cpu, (u16)(STACK + cpu->sp));
}

void cpu_stack_push_u16(cpu_t *cpu, u16 data) {
	u8 hi = data >> 8;
	u8 lo = data & 0xff;
	cpu_stack_push(cpu, hi);
	cpu_stack_push(cpu, lo);
}

u16 cpu_stack_pop_u16(cpu_t *cpu) {
	u16 lo = (u16) cpu_stack_pop(cpu);
	u16 hi = (u16) cpu_stack_pop(cpu);

	return hi << 8 | lo;
}

void cpu_reset(cpu_t *cpu) {
	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;
	cpu->sp = STACK_RESET;
	cpu->sr = 0b100100;

	cpu->pc = cpu_mem_read_u16(cpu, 0xFFFC);
}

void cpu_load(cpu_t *cpu, u8 *program, int size) {
	for (int i = 0; i < size; i++) {
		cpu->memory[0x0600 + i] = program[i];
	}

	cpu_mem_write_u16(cpu, 0xFFFC, 0x0600);
}

void cpu_run(cpu_t *cpu) {
	cpu_run_with_callback(cpu, NULL);
}

void cpu_run_with_callback(cpu_t *cpu, void (*callback)(cpu_t *cpu)) {
	while (true) {
		if (callback != NULL) {
			callback(cpu);
		}

		u8 code = cpu_mem_read(cpu, cpu->pc);
		cpu->pc += 1;
		u16 pc_state = cpu->pc;

		opcode_t opcode = optable[code];

		if (code == 0x00) break;
		else if (code == 0xea) {
			// "NOP" means do nothing
		}
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
