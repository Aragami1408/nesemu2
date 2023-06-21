#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// CPU UTILS

static opcode_t optable[0xFF + 1];

#define OPTABLE_ASSIGN(__code, __mnemonic, __bytes, __cycles, __is_implied, __func, __mode) \
	optable[(__code)].code = (__code); \
	strcpy(optable[(__code)].mnemonic, (__mnemonic)); \
	optable[(__code)].bytes = (__bytes); \
	optable[(__code)].cycles = (__cycles); \
	if((__is_implied)) { \
		optable[(__code)].implied_opcode = (__func); \
	}\
	else {\
		optable[(__code)].nimplied_opcode = (__func); \
	}\
	optable[(__code)].mode = (__mode); \


static void optable_generator(void) {
	optable[0x00] = (opcode_t){.code = 0x00, .bytes = 1, .cycles = 7, .nimplied_opcode = NULL, .mode = NONE};
	strcpy(optable[0x00].mnemonic, "BRK");

	optable[0xea] = (opcode_t){.code = 0xea, .bytes = 1, .cycles = 2, .nimplied_opcode = NULL, .mode = NONE};
	strcpy(optable[0xea].mnemonic, "NOP");

	optable[0x69] = (opcode_t){.code = 0x69, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_adc, .mode = IMM};
	strcpy(optable[0x69].mnemonic, "ADC");

	optable[0x65] = (opcode_t){.code = 0x65, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_adc, .mode = ZP};
	strcpy(optable[0x65].mnemonic, "ADC");

	optable[0x75] = (opcode_t){.code = 0x75, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_adc, .mode = ZPX};
	strcpy(optable[0x75].mnemonic, "ADC");

	optable[0x6d] = (opcode_t){.code = 0x6d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_adc, .mode = ABS};
	strcpy(optable[0x6d].mnemonic, "ADC");

	optable[0x7d] = (opcode_t){.code = 0x7d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_adc, .mode = ABSX};
	strcpy(optable[0x7d].mnemonic, "ADC");

	optable[0x79] = (opcode_t){.code = 0x79, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_adc, .mode = ABSY};
	strcpy(optable[0x79].mnemonic, "ADC");

	optable[0x61] = (opcode_t){.code = 0x61, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_adc, .mode = INDX};
	strcpy(optable[0x61].mnemonic, "ADC");

	optable[0x71] = (opcode_t){.code = 0x71, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_adc, .mode = INDY};
	strcpy(optable[0x71].mnemonic, "ADC");

	optable[0xe9] = (opcode_t){.code = 0xe9, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_sbc, .mode = IMM};
	strcpy(optable[0xe9].mnemonic, "SBC");

	optable[0xe5] = (opcode_t){.code = 0xe5, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_sbc, .mode = ZP};
	strcpy(optable[0xe5].mnemonic, "SBC");

	optable[0xf5] = (opcode_t){.code = 0xf5, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_sbc, .mode = ZPX};
	strcpy(optable[0xf5].mnemonic, "SBC");

	optable[0xed] = (opcode_t){.code = 0xed, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_sbc, .mode = ABS};
	strcpy(optable[0xed].mnemonic, "SBC");

	optable[0xfd] = (opcode_t){.code = 0xfd, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_sbc, .mode = ABSX};
	strcpy(optable[0xfd].mnemonic, "SBC");

	optable[0xf9] = (opcode_t){.code = 0xf9, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_sbc, .mode = ABSY};
	strcpy(optable[0xf9].mnemonic, "SBC");

	optable[0xe1] = (opcode_t){.code = 0xe1, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_sbc, .mode = INDX};
	strcpy(optable[0xe1].mnemonic, "SBC");

	optable[0xf1] = (opcode_t){.code = 0xf1, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_sbc, .mode = INDY};
	strcpy(optable[0xf1].mnemonic, "SBC");

	optable[0xc9] = (opcode_t){.code = 0xc9, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_cmp, .mode = IMM};
	strcpy(optable[0xc9].mnemonic, "CMP");

	optable[0xc5] = (opcode_t){.code = 0xc5, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_cmp, .mode = ZP};
	strcpy(optable[0xc5].mnemonic, "CMP");

	optable[0xd5] = (opcode_t){.code = 0xd5, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_cmp, .mode = ZPX};
	strcpy(optable[0xd5].mnemonic, "CMP");

	optable[0xcd] = (opcode_t){.code = 0xcd, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_cmp, .mode = ABS};
	strcpy(optable[0xcd].mnemonic, "CMP");

	optable[0xdd] = (opcode_t){.code = 0xdd, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_cmp, .mode = ABSX};
	strcpy(optable[0xdd].mnemonic, "CMP");

	optable[0xd9] = (opcode_t){.code = 0xd9, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_cmp, .mode = ABSY};
	strcpy(optable[0xd9].mnemonic, "CMP");

	optable[0xc1] = (opcode_t){.code = 0xc1, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_cmp, .mode = INDX};
	strcpy(optable[0xc1].mnemonic, "CMP");

	optable[0xd1] = (opcode_t){.code = 0xd1, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_cmp, .mode = INDY};
	strcpy(optable[0xd1].mnemonic, "CMP");

	optable[0xe0] = (opcode_t){.code = 0xe0, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_cpx, .mode = IMM};
	strcpy(optable[0xe0].mnemonic, "CPX");

	optable[0xe4] = (opcode_t){.code = 0xe4, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_cpx, .mode = ZP};
	strcpy(optable[0xe4].mnemonic, "CPX");

	optable[0xec] = (opcode_t){.code = 0xec, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_cpx, .mode = ABS};
	strcpy(optable[0xec].mnemonic, "CPX");

	optable[0xc0] = (opcode_t){.code = 0xc0, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_cpy, .mode = IMM};
	strcpy(optable[0xc0].mnemonic, "CPY");

	optable[0xc4] = (opcode_t){.code = 0xc4, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_cpy, .mode = ZP};
	strcpy(optable[0xc4].mnemonic, "CPY");

	optable[0xcc] = (opcode_t){.code = 0xcc, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_cpy, .mode = ABS};
	strcpy(optable[0xcc].mnemonic, "CPY");

	optable[0x29] = (opcode_t){.code = 0x29, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_and, .mode = IMM};
	strcpy(optable[0x29].mnemonic, "AND");

	optable[0x25] = (opcode_t){.code = 0x25, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_and, .mode = ZP};
	strcpy(optable[0x25].mnemonic, "AND");

	optable[0x35] = (opcode_t){.code = 0x35, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_and, .mode = ZPX};
	strcpy(optable[0x35].mnemonic, "AND");

	optable[0x2d] = (opcode_t){.code = 0x2d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_and, .mode = ABS};
	strcpy(optable[0x2d].mnemonic, "AND");

	optable[0x3d] = (opcode_t){.code = 0x3d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_and, .mode = ABSX};
	strcpy(optable[0x3d].mnemonic, "AND");

	optable[0x39] = (opcode_t){.code = 0x39, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_and, .mode = ABSY};
	strcpy(optable[0x39].mnemonic, "AND");

	optable[0x21] = (opcode_t){.code = 0x21, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_and, .mode = INDX};
	strcpy(optable[0x21].mnemonic, "AND");

	optable[0x31] = (opcode_t){.code = 0x31, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_and, .mode = INDY};
	strcpy(optable[0x31].mnemonic, "AND");

	optable[0x49] = (opcode_t){.code = 0x49, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_eor, .mode = IMM};
	strcpy(optable[0x49].mnemonic, "EOR");

	optable[0x45] = (opcode_t){.code = 0x45, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_eor, .mode = ZP};
	strcpy(optable[0x45].mnemonic, "EOR");

	optable[0x55] = (opcode_t){.code = 0x55, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_eor, .mode = ZPX};
	strcpy(optable[0x55].mnemonic, "EOR");

	optable[0x4d] = (opcode_t){.code = 0x4d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_eor, .mode = ABS};
	strcpy(optable[0x4d].mnemonic, "EOR");

	optable[0x5d] = (opcode_t){.code = 0x5d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_eor, .mode = ABSX};
	strcpy(optable[0x5d].mnemonic, "EOR");

	optable[0x59] = (opcode_t){.code = 0x59, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_eor, .mode = ABSY};
	strcpy(optable[0x59].mnemonic, "EOR");

	optable[0x41] = (opcode_t){.code = 0x41, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_eor, .mode = INDX};
	strcpy(optable[0x41].mnemonic, "EOR");

	optable[0x51] = (opcode_t){.code = 0x51, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_eor, .mode = ABSY};
	strcpy(optable[0x51].mnemonic, "EOR");

	optable[0x09] = (opcode_t){.code = 0x09, .bytes = 2, .cycles = 2, .nimplied_opcode = &opcode_ora, .mode = IMM};
	strcpy(optable[0x09].mnemonic, "ORA");

	optable[0x05] = (opcode_t){.code = 0x05, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_ora, .mode = ZP};
	strcpy(optable[0x05].mnemonic, "ORA");

	optable[0x15] = (opcode_t){.code = 0x15, .bytes = 2, .cycles = 4, .nimplied_opcode = &opcode_ora, .mode = ZPX};
	strcpy(optable[0x15].mnemonic, "ORA");

	optable[0x0d] = (opcode_t){.code = 0x0d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_ora, .mode = ABS};
	strcpy(optable[0x0d].mnemonic, "ORA");

	optable[0x1d] = (opcode_t){.code = 0x1d, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_ora, .mode = ABSX};
	strcpy(optable[0x1d].mnemonic, "ORA");

	optable[0x19] = (opcode_t){.code = 0x19, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_ora, .mode = ABSY};
	strcpy(optable[0x19].mnemonic, "ORA");

	optable[0x01] = (opcode_t){.code = 0x01, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_ora, .mode = INDX};
	strcpy(optable[0x01].mnemonic, "ORA");

	optable[0x11] = (opcode_t){.code = 0x11, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_ora, .mode = INDY};
	strcpy(optable[0x11].mnemonic, "ORA");

	optable[0x24] = (opcode_t){.code = 0x24, .bytes = 2, .cycles = 3, .nimplied_opcode = &opcode_bit, .mode = ZP};
	strcpy(optable[0x24].mnemonic, "BIT");

	optable[0x2c] = (opcode_t){.code = 0x2c, .bytes = 3, .cycles = 4, .nimplied_opcode = &opcode_bit, .mode = ABS};
	strcpy(optable[0x2c].mnemonic, "BIT");

	optable[0xe6] = (opcode_t){.code = 0xe6, .bytes = 2, .cycles = 5, .nimplied_opcode = &opcode_inc, .mode = ZP};
	strcpy(optable[0xe6].mnemonic, "INC");

	optable[0xf6] = (opcode_t){.code = 0xf6, .bytes = 2, .cycles = 6, .nimplied_opcode = &opcode_inc, .mode = ZPX};
	strcpy(optable[0xf6].mnemonic, "INC");

	optable[0xee] = (opcode_t){.code = 0xee, .bytes = 3, .cycles = 6, .nimplied_opcode = &opcode_inc, .mode = ABS};
	strcpy(optable[0xee].mnemonic, "INC");

	optable[0xfe] = (opcode_t){.code = 0xfe, .bytes = 3, .cycles = 7, .nimplied_opcode = &opcode_inc, .mode = ABSX};
	strcpy(optable[0xfe].mnemonic, "INC");

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

cpu_t *cpu_init(void) {
	cpu_t *cpu = (cpu_t *) malloc(sizeof(cpu_t));
	if (cpu == NULL) {
		return NULL;
	}

	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;

	cpu->pc = 0;
	cpu->sp = STACK_RESET;
	cpu->sr = 0x24;

	for (u16 i = 0; i < MEMORY_MAX; i++) cpu->memory[i] = 0;
	
	optable_generator();

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
	u8 hi = (u8)  (data >> 8);
	u8 lo = (u8) (data & 0xff);
	cpu_mem_write(cpu, pos, lo);
	cpu_mem_write(cpu, pos + 1, hi);
}

void cpu_stack_push(cpu_t *cpu, u8 data) {
	cpu_mem_write(cpu, STACK + cpu->sp, data);
	cpu->sp -= 1;
}

u8 cpu_stack_pop(cpu_t *cpu) {
	cpu->sp++;
	return cpu_mem_read(cpu, STACK + cpu->sp);
}

void cpu_stack_push_u16(cpu_t *cpu, u16 data) {
	u8 hi = (u8) (data >> 8);
	u8 lo = (u8) (data & 0xff);
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
	cpu->sr = 0x24;

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

		u8 code = cpu_mem_read(cpu, cpu->pc);
		cpu->pc += 1;
		u16 pc_state = cpu->pc;

		opcode_t opcode = optable[code];
		// cpu_mem_write(cpu, 0xfe, (u8) (rand() % (16 - 1) + 1));

		if (code == 0x00) break;
		else if (code == 0xea) {
			// "NOP" means do nothing
		}
		// else if (opcode.implied_opcode == NULL || opcode.nimplied_opcode == NULL) {
		// 	break;
		// }
		else if (opcode.implied_opcode != NULL) {
			opcode.implied_opcode(cpu);
		}
		else if (opcode.nimplied_opcode != NULL) {
			opcode.nimplied_opcode(cpu, opcode.mode);
		}
		else {
			break;
		}
		
		if (pc_state == cpu->pc) cpu->pc += (u16) (opcode.bytes - 1);

		if (callback != NULL) {
			callback(cpu);
		}
	}

}

void cpu_load_and_run(cpu_t *cpu, u8 *program, int size) {
	cpu_load(cpu, program, size);
	cpu->pc = cpu_mem_read_u16(cpu, 0xfffc);
	cpu_run(cpu);
}

void cpu_free(cpu_t *cpu) {
	free(cpu);
}
