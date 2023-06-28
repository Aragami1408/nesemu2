#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// CPU UTILS

static opcode_t optable[0xFF + 1];

static void optable_generator(void) {
	optable[0x00] = (opcode_t){.code = 0x00, .bytes = 1, .cycles = 7, .func = NULL, .mode = NONE};
	strcpy(optable[0x00].mnemonic, "BRK");

	optable[0xea] = (opcode_t){.code = 0xea, .bytes = 1, .cycles = 2, .func = NULL, .mode = NONE};
	strcpy(optable[0xea].mnemonic, "NOP");

	optable[0x69] = (opcode_t){.code = 0x69, .bytes = 2, .cycles = 2, .func = &opcode_adc, .mode = IMM};
	strcpy(optable[0x69].mnemonic, "ADC");

	optable[0x65] = (opcode_t){.code = 0x65, .bytes = 2, .cycles = 3, .func = &opcode_adc, .mode = ZP};
	strcpy(optable[0x65].mnemonic, "ADC");

	optable[0x75] = (opcode_t){.code = 0x75, .bytes = 2, .cycles = 4, .func = &opcode_adc, .mode = ZPX};
	strcpy(optable[0x75].mnemonic, "ADC");

	optable[0x6d] = (opcode_t){.code = 0x6d, .bytes = 3, .cycles = 4, .func = &opcode_adc, .mode = ABS};
	strcpy(optable[0x6d].mnemonic, "ADC");

	optable[0x7d] = (opcode_t){.code = 0x7d, .bytes = 3, .cycles = 4, .func = &opcode_adc, .mode = ABSX};
	strcpy(optable[0x7d].mnemonic, "ADC");

	optable[0x79] = (opcode_t){.code = 0x79, .bytes = 3, .cycles = 4, .func = &opcode_adc, .mode = ABSY};
	strcpy(optable[0x79].mnemonic, "ADC");

	optable[0x61] = (opcode_t){.code = 0x61, .bytes = 2, .cycles = 6, .func = &opcode_adc, .mode = INDX};
	strcpy(optable[0x61].mnemonic, "ADC");

	optable[0x71] = (opcode_t){.code = 0x71, .bytes = 2, .cycles = 5, .func = &opcode_adc, .mode = INDY};
	strcpy(optable[0x71].mnemonic, "ADC");

	optable[0xe9] = (opcode_t){.code = 0xe9, .bytes = 2, .cycles = 2, .func = &opcode_sbc, .mode = IMM};
	strcpy(optable[0xe9].mnemonic, "SBC");

	optable[0xe5] = (opcode_t){.code = 0xe5, .bytes = 2, .cycles = 3, .func = &opcode_sbc, .mode = ZP};
	strcpy(optable[0xe5].mnemonic, "SBC");

	optable[0xf5] = (opcode_t){.code = 0xf5, .bytes = 2, .cycles = 4, .func = &opcode_sbc, .mode = ZPX};
	strcpy(optable[0xf5].mnemonic, "SBC");

	optable[0xed] = (opcode_t){.code = 0xed, .bytes = 3, .cycles = 4, .func = &opcode_sbc, .mode = ABS};
	strcpy(optable[0xed].mnemonic, "SBC");

	optable[0xfd] = (opcode_t){.code = 0xfd, .bytes = 3, .cycles = 4, .func = &opcode_sbc, .mode = ABSX};
	strcpy(optable[0xfd].mnemonic, "SBC");

	optable[0xf9] = (opcode_t){.code = 0xf9, .bytes = 3, .cycles = 4, .func = &opcode_sbc, .mode = ABSY};
	strcpy(optable[0xf9].mnemonic, "SBC");

	optable[0xe1] = (opcode_t){.code = 0xe1, .bytes = 2, .cycles = 6, .func = &opcode_sbc, .mode = INDX};
	strcpy(optable[0xe1].mnemonic, "SBC");

	optable[0xf1] = (opcode_t){.code = 0xf1, .bytes = 2, .cycles = 5, .func = &opcode_sbc, .mode = INDY};
	strcpy(optable[0xf1].mnemonic, "SBC");

	optable[0xc9] = (opcode_t){.code = 0xc9, .bytes = 2, .cycles = 2, .func = &opcode_cmp, .mode = IMM};
	strcpy(optable[0xc9].mnemonic, "CMP");

	optable[0xc5] = (opcode_t){.code = 0xc5, .bytes = 2, .cycles = 3, .func = &opcode_cmp, .mode = ZP};
	strcpy(optable[0xc5].mnemonic, "CMP");

	optable[0xd5] = (opcode_t){.code = 0xd5, .bytes = 2, .cycles = 4, .func = &opcode_cmp, .mode = ZPX};
	strcpy(optable[0xd5].mnemonic, "CMP");

	optable[0xcd] = (opcode_t){.code = 0xcd, .bytes = 3, .cycles = 4, .func = &opcode_cmp, .mode = ABS};
	strcpy(optable[0xcd].mnemonic, "CMP");

	optable[0xdd] = (opcode_t){.code = 0xdd, .bytes = 3, .cycles = 4, .func = &opcode_cmp, .mode = ABSX};
	strcpy(optable[0xdd].mnemonic, "CMP");

	optable[0xd9] = (opcode_t){.code = 0xd9, .bytes = 3, .cycles = 4, .func = &opcode_cmp, .mode = ABSY};
	strcpy(optable[0xd9].mnemonic, "CMP");

	optable[0xc1] = (opcode_t){.code = 0xc1, .bytes = 2, .cycles = 6, .func = &opcode_cmp, .mode = INDX};
	strcpy(optable[0xc1].mnemonic, "CMP");

	optable[0xd1] = (opcode_t){.code = 0xd1, .bytes = 2, .cycles = 5, .func = &opcode_cmp, .mode = INDY};
	strcpy(optable[0xd1].mnemonic, "CMP");

	optable[0xe0] = (opcode_t){.code = 0xe0, .bytes = 2, .cycles = 2, .func = &opcode_cpx, .mode = IMM};
	strcpy(optable[0xe0].mnemonic, "CPX");

	optable[0xe4] = (opcode_t){.code = 0xe4, .bytes = 2, .cycles = 3, .func = &opcode_cpx, .mode = ZP};
	strcpy(optable[0xe4].mnemonic, "CPX");

	optable[0xec] = (opcode_t){.code = 0xec, .bytes = 3, .cycles = 4, .func = &opcode_cpx, .mode = ABS};
	strcpy(optable[0xec].mnemonic, "CPX");

	optable[0xc0] = (opcode_t){.code = 0xc0, .bytes = 2, .cycles = 2, .func = &opcode_cpy, .mode = IMM};
	strcpy(optable[0xc0].mnemonic, "CPY");

	optable[0xc4] = (opcode_t){.code = 0xc4, .bytes = 2, .cycles = 3, .func = &opcode_cpy, .mode = ZP};
	strcpy(optable[0xc4].mnemonic, "CPY");

	optable[0xcc] = (opcode_t){.code = 0xcc, .bytes = 3, .cycles = 4, .func = &opcode_cpy, .mode = ABS};
	strcpy(optable[0xcc].mnemonic, "CPY");

	optable[0x29] = (opcode_t){.code = 0x29, .bytes = 2, .cycles = 2, .func = &opcode_and, .mode = IMM};
	strcpy(optable[0x29].mnemonic, "AND");

	optable[0x25] = (opcode_t){.code = 0x25, .bytes = 2, .cycles = 3, .func = &opcode_and, .mode = ZP};
	strcpy(optable[0x25].mnemonic, "AND");

	optable[0x35] = (opcode_t){.code = 0x35, .bytes = 2, .cycles = 4, .func = &opcode_and, .mode = ZPX};
	strcpy(optable[0x35].mnemonic, "AND");

	optable[0x2d] = (opcode_t){.code = 0x2d, .bytes = 3, .cycles = 4, .func = &opcode_and, .mode = ABS};
	strcpy(optable[0x2d].mnemonic, "AND");

	optable[0x3d] = (opcode_t){.code = 0x3d, .bytes = 3, .cycles = 4, .func = &opcode_and, .mode = ABSX};
	strcpy(optable[0x3d].mnemonic, "AND");

	optable[0x39] = (opcode_t){.code = 0x39, .bytes = 3, .cycles = 4, .func = &opcode_and, .mode = ABSY};
	strcpy(optable[0x39].mnemonic, "AND");

	optable[0x21] = (opcode_t){.code = 0x21, .bytes = 2, .cycles = 6, .func = &opcode_and, .mode = INDX};
	strcpy(optable[0x21].mnemonic, "AND");

	optable[0x31] = (opcode_t){.code = 0x31, .bytes = 2, .cycles = 5, .func = &opcode_and, .mode = INDY};
	strcpy(optable[0x31].mnemonic, "AND");

	optable[0x49] = (opcode_t){.code = 0x49, .bytes = 2, .cycles = 2, .func = &opcode_eor, .mode = IMM};
	strcpy(optable[0x49].mnemonic, "EOR");

	optable[0x45] = (opcode_t){.code = 0x45, .bytes = 2, .cycles = 3, .func = &opcode_eor, .mode = ZP};
	strcpy(optable[0x45].mnemonic, "EOR");

	optable[0x55] = (opcode_t){.code = 0x55, .bytes = 2, .cycles = 4, .func = &opcode_eor, .mode = ZPX};
	strcpy(optable[0x55].mnemonic, "EOR");

	optable[0x4d] = (opcode_t){.code = 0x4d, .bytes = 3, .cycles = 4, .func = &opcode_eor, .mode = ABS};
	strcpy(optable[0x4d].mnemonic, "EOR");

	optable[0x5d] = (opcode_t){.code = 0x5d, .bytes = 3, .cycles = 4, .func = &opcode_eor, .mode = ABSX};
	strcpy(optable[0x5d].mnemonic, "EOR");

	optable[0x59] = (opcode_t){.code = 0x59, .bytes = 3, .cycles = 4, .func = &opcode_eor, .mode = ABSY};
	strcpy(optable[0x59].mnemonic, "EOR");

	optable[0x41] = (opcode_t){.code = 0x41, .bytes = 2, .cycles = 6, .func = &opcode_eor, .mode = INDX};
	strcpy(optable[0x41].mnemonic, "EOR");

	optable[0x51] = (opcode_t){.code = 0x51, .bytes = 2, .cycles = 5, .func = &opcode_eor, .mode = ABSY};
	strcpy(optable[0x51].mnemonic, "EOR");

	optable[0x09] = (opcode_t){.code = 0x09, .bytes = 2, .cycles = 2, .func = &opcode_ora, .mode = IMM};
	strcpy(optable[0x09].mnemonic, "ORA");

	optable[0x05] = (opcode_t){.code = 0x05, .bytes = 2, .cycles = 3, .func = &opcode_ora, .mode = ZP};
	strcpy(optable[0x05].mnemonic, "ORA");

	optable[0x15] = (opcode_t){.code = 0x15, .bytes = 2, .cycles = 4, .func = &opcode_ora, .mode = ZPX};
	strcpy(optable[0x15].mnemonic, "ORA");

	optable[0x0d] = (opcode_t){.code = 0x0d, .bytes = 3, .cycles = 4, .func = &opcode_ora, .mode = ABS};
	strcpy(optable[0x0d].mnemonic, "ORA");

	optable[0x1d] = (opcode_t){.code = 0x1d, .bytes = 3, .cycles = 4, .func = &opcode_ora, .mode = ABSX};
	strcpy(optable[0x1d].mnemonic, "ORA");

	optable[0x19] = (opcode_t){.code = 0x19, .bytes = 3, .cycles = 4, .func = &opcode_ora, .mode = ABSY};
	strcpy(optable[0x19].mnemonic, "ORA");

	optable[0x01] = (opcode_t){.code = 0x01, .bytes = 2, .cycles = 6, .func = &opcode_ora, .mode = INDX};
	strcpy(optable[0x01].mnemonic, "ORA");

	optable[0x11] = (opcode_t){.code = 0x11, .bytes = 2, .cycles = 5, .func = &opcode_ora, .mode = INDY};
	strcpy(optable[0x11].mnemonic, "ORA");

	optable[0x24] = (opcode_t){.code = 0x24, .bytes = 2, .cycles = 3, .func = &opcode_bit, .mode = ZP};
	strcpy(optable[0x24].mnemonic, "BIT");

	optable[0x2c] = (opcode_t){.code = 0x2c, .bytes = 3, .cycles = 4, .func = &opcode_bit, .mode = ABS};
	strcpy(optable[0x2c].mnemonic, "BIT");

	optable[0xe6] = (opcode_t){.code = 0xe6, .bytes = 2, .cycles = 5, .func = &opcode_inc, .mode = ZP};
	strcpy(optable[0xe6].mnemonic, "INC");

	optable[0xf6] = (opcode_t){.code = 0xf6, .bytes = 2, .cycles = 6, .func = &opcode_inc, .mode = ZPX};
	strcpy(optable[0xf6].mnemonic, "INC");

	optable[0xee] = (opcode_t){.code = 0xee, .bytes = 3, .cycles = 6, .func = &opcode_inc, .mode = ABS};
	strcpy(optable[0xee].mnemonic, "INC");

	optable[0xfe] = (opcode_t){.code = 0xfe, .bytes = 3, .cycles = 7, .func = &opcode_inc, .mode = ABSX};
	strcpy(optable[0xfe].mnemonic, "INC");

	optable[0xe8] = (opcode_t){.code = 0xe8, .bytes = 1, .cycles = 2, .func = &opcode_inx, .mode = NONE};
	strcpy(optable[0xe8].mnemonic, "INX");

	optable[0xc8] = (opcode_t){.code = 0xc8, .bytes = 1, .cycles = 2, .func = &opcode_iny, .mode = NONE};
	strcpy(optable[0xc8].mnemonic, "INY");

	optable[0xc6] = (opcode_t){.code = 0xc6, .bytes = 2, .cycles = 5, .func = &opcode_dec, .mode = ZP};
	strcpy(optable[0xc6].mnemonic, "DEC");

	optable[0xd6] = (opcode_t){.code = 0xd6, .bytes = 2, .cycles = 6, .func = &opcode_dec, .mode = ZPX};
	strcpy(optable[0xd6].mnemonic, "DEC");

	optable[0xce] = (opcode_t){.code = 0xce, .bytes = 3, .cycles = 6, .func = &opcode_dec, .mode = ABS};
	strcpy(optable[0xce].mnemonic, "DEC");

	optable[0xde] = (opcode_t){.code = 0xde, .bytes = 3, .cycles = 7, .func = &opcode_dec, .mode = ABSX};
	strcpy(optable[0xde].mnemonic, "DEC");

	optable[0x0a] = (opcode_t){.code = 0x0a, .bytes = 1, .cycles = 2, .func = &opcode_asl, .mode = ACC};
strcpy(optable[0x0a].mnemonic, "ASL");

	optable[0x06] = (opcode_t){.code = 0x06, .bytes = 2, .cycles = 5, .func = &opcode_asl, .mode = ZP};
strcpy(optable[0x06].mnemonic, "ASL");

	optable[0x16] = (opcode_t){.code = 0x16, .bytes = 2, .cycles = 6, .func = &opcode_asl, .mode = ZPX};
strcpy(optable[0x16].mnemonic, "ASL");

	optable[0x0e] = (opcode_t){.code = 0x0e, .bytes = 3, .cycles = 6, .func = &opcode_asl, .mode = ABS};
strcpy(optable[0x0e].mnemonic, "ASL");

	optable[0x1e] = (opcode_t){.code = 0x1e, .bytes = 3, .cycles = 7, .func = &opcode_asl, .mode = ABSX};
strcpy(optable[0x1e].mnemonic, "ASL");


	optable[0x4a] = (opcode_t){.code = 0x4a, .bytes = 1, .cycles = 2, .func = &opcode_lsr, .mode = ACC};
strcpy(optable[0x4a].mnemonic, "LSR");

	optable[0x46] = (opcode_t){.code = 0x46, .bytes = 2, .cycles = 5, .func = &opcode_lsr, .mode = ZP};
strcpy(optable[0x46].mnemonic, "LSR");

	optable[0x56] = (opcode_t){.code = 0x56, .bytes = 2, .cycles = 6, .func = &opcode_lsr, .mode = ZPX};
strcpy(optable[0x56].mnemonic, "LSR");

	optable[0x4e] = (opcode_t){.code = 0x4e, .bytes = 3, .cycles = 6, .func = &opcode_lsr, .mode = ABS};
strcpy(optable[0x4e].mnemonic, "LSR");

	optable[0x5e] = (opcode_t){.code = 0x5e, .bytes = 3, .cycles = 7, .func = &opcode_lsr, .mode = ABSX};
strcpy(optable[0x5e].mnemonic, "LSR");

	optable[0x2a] = (opcode_t){.code = 0x2a, .bytes = 1, .cycles = 2, .func = &opcode_rol, .mode = ACC};
strcpy(optable[0x2a].mnemonic, "ROL");

	optable[0x26] = (opcode_t){.code = 0x26, .bytes = 2, .cycles = 5, .func = &opcode_rol, .mode = ZP};
strcpy(optable[0x26].mnemonic, "ROL");

	optable[0x36] = (opcode_t){.code = 0x36, .bytes = 2, .cycles = 6, .func = &opcode_rol, .mode = ZPX};
strcpy(optable[0x36].mnemonic, "ROL");

	optable[0x2e] = (opcode_t){.code = 0x2e, .bytes = 3, .cycles = 6, .func = &opcode_rol, .mode = ABS};
strcpy(optable[0x2e].mnemonic, "ROL");

	optable[0x3e] = (opcode_t){.code = 0x3e, .bytes = 3, .cycles = 7, .func = &opcode_rol, .mode = ABSX};
strcpy(optable[0x3e].mnemonic, "ROL");


	optable[0xaa] = (opcode_t){.code = 0xaa, .bytes = 1, .cycles = 2, .func = &opcode_tax, .mode = NONE};
strcpy(optable[0xaa].mnemonic, "TAX");
	
	optable[0xa8] = (opcode_t){.code = 0xa8, .bytes = 1, .cycles = 2, .func = &opcode_tay, .mode = NONE};
strcpy(optable[0xa8].mnemonic, "TAY");
	
	optable[0x8a] = (opcode_t){.code = 0x8a, .bytes = 1, .cycles = 2, .func = &opcode_txa, .mode = NONE};
strcpy(optable[0x8a].mnemonic, "TXA");
	
	optable[0x98] = (opcode_t){.code = 0x98, .bytes = 1, .cycles = 2, .func = &opcode_tya, .mode = NONE};
strcpy(optable[0x98].mnemonic, "TYA");
	

	optable[0xba] = (opcode_t){.code = 0xba, .bytes = 1, .cycles = 2, .func = &opcode_tsx, .mode = NONE};
strcpy(optable[0xba].mnemonic, "TSX");

	optable[0x9a] = (opcode_t){.code = 0x9a, .bytes = 1, .cycles = 2, .func = &opcode_txs, .mode = NONE};
strcpy(optable[0x9a].mnemonic, "TXS");

	optable[0x48] = (opcode_t){.code = 0x48, .bytes = 1, .cycles = 3, .func = &opcode_pha, .mode = NONE};
strcpy(optable[0x48].mnemonic, "PHA");

	optable[0x08] = (opcode_t){.code = 0x08, .bytes = 1, .cycles = 3, .func = &opcode_php, .mode = NONE};
strcpy(optable[0x08].mnemonic, "PHP");

	optable[0x68] = (opcode_t){.code = 0x68, .bytes = 1, .cycles = 4, .func = &opcode_pla, .mode = NONE};
strcpy(optable[0x68].mnemonic, "PLA");

	optable[0x28] = (opcode_t){.code = 0x28, .bytes = 1, .cycles = 4, .func = &opcode_plp, .mode = NONE};
strcpy(optable[0x28].mnemonic, "PLP");


	optable[0x4c] = (opcode_t){.code = 0x4c, .bytes = 3, .cycles = 3, .func = &opcode_jmp, .mode = ABS};
strcpy(optable[0x4c].mnemonic, "JMP");

	optable[0x6c] = (opcode_t){.code = 0x6c, .bytes = 3, .cycles = 5, .func = &opcode_jmp, .mode = IND};
strcpy(optable[0x6c].mnemonic, "JMP");


	optable[0x20] = (opcode_t){.code = 0x20, .bytes = 3, .cycles = 6, .func = &opcode_jsr, .mode = ABS};
strcpy(optable[0x20].mnemonic, "JSR");

	optable[0x60] = (opcode_t){.code = 0x60, .bytes = 1, .cycles = 6, .func = &opcode_rts, .mode = NONE};
strcpy(optable[0x60].mnemonic, "RTS");


	optable[0x90] = (opcode_t){.code = 0x90, .bytes = 2, .cycles = 2, .func = &opcode_bcc, .mode = NONE};
strcpy(optable[0x90].mnemonic, "BCC");

	optable[0xb0] = (opcode_t){.code = 0xb0, .bytes = 2, .cycles = 2, .func = &opcode_bcs, .mode = NONE};
strcpy(optable[0xb0].mnemonic, "BCS");

	optable[0xf0] = (opcode_t){.code = 0xf0, .bytes = 2, .cycles = 2, .func = &opcode_beq, .mode = NONE};
strcpy(optable[0xf0].mnemonic, "BEQ");

	optable[0x30] = (opcode_t){.code = 0x30, .bytes = 2, .cycles = 2, .func = &opcode_bmi, .mode = NONE};
strcpy(optable[0x30].mnemonic, "BMI");

	optable[0xd0] = (opcode_t){.code = 0xd0, .bytes = 2, .cycles = 2, .func = &opcode_bne, .mode = NONE};
strcpy(optable[0xd0].mnemonic, "BNE");

	optable[0x10] = (opcode_t){.code = 0x10, .bytes = 2, .cycles = 2, .func = &opcode_bpl, .mode = NONE};
strcpy(optable[0x10].mnemonic, "BPL");

	optable[0x50] = (opcode_t){.code = 0x50, .bytes = 2, .cycles = 2, .func = &opcode_bvc, .mode = NONE};
strcpy(optable[0x50].mnemonic, "BVC");

	optable[0x70] = (opcode_t){.code = 0x70, .bytes = 2, .cycles = 2, .func = &opcode_bvs, .mode = NONE};
strcpy(optable[0x70].mnemonic, "BVS");


	optable[0x18] = (opcode_t){.code = 0x18, .bytes = 1, .cycles = 2, .func = &opcode_clc, .mode = NONE};
strcpy(optable[0x18].mnemonic, "CLC");

	optable[0xd8] = (opcode_t){.code = 0xd8, .bytes = 1, .cycles = 2, .func = &opcode_cld, .mode = NONE};
strcpy(optable[0xd8].mnemonic, "CLD");

	optable[0x58] = (opcode_t){.code = 0x58, .bytes = 1, .cycles = 2, .func = &opcode_cli, .mode = NONE};
strcpy(optable[0x58].mnemonic, "CLI");

	optable[0xb8] = (opcode_t){.code = 0xb8, .bytes = 1, .cycles = 2, .func = &opcode_clv, .mode = NONE};
strcpy(optable[0xb8].mnemonic, "CLV");

	optable[0x38] = (opcode_t){.code = 0x38, .bytes = 1, .cycles = 2, .func = &opcode_sec, .mode = NONE};
strcpy(optable[0x38].mnemonic, "SEC");

	optable[0xf8] = (opcode_t){.code = 0xf8, .bytes = 1, .cycles = 2, .func = &opcode_sed, .mode = NONE};
strcpy(optable[0xf8].mnemonic, "SED");

	optable[0x78] = (opcode_t){.code = 0x78, .bytes = 1, .cycles = 2, .func = &opcode_sei, .mode = NONE};
strcpy(optable[0x78].mnemonic, "SEI");



	optable[0xa9] = (opcode_t){.code = 0xa9, .bytes = 2, .cycles = 2, .func = &opcode_lda, .mode = IMM};
strcpy(optable[0xa9].mnemonic, "LDA");

	optable[0xa5] = (opcode_t){.code = 0xa5, .bytes = 2, .cycles = 3, .func = &opcode_lda, .mode = ZP};
strcpy(optable[0xa5].mnemonic, "LDA");

	optable[0xb5] = (opcode_t){.code = 0xb5, .bytes = 2, .cycles = 4, .func = &opcode_lda, .mode = ZPX};
strcpy(optable[0xb5].mnemonic, "LDA");

	optable[0xad] = (opcode_t){.code = 0xad, .bytes = 3, .cycles = 4, .func = &opcode_lda, .mode = ABS};
strcpy(optable[0xad].mnemonic, "LDA");

	optable[0xbd] = (opcode_t){.code = 0xbd, .bytes = 3, .cycles = 4, .func = &opcode_lda, .mode = ABSX};
strcpy(optable[0xbd].mnemonic, "LDA");

	optable[0xb9] = (opcode_t){.code = 0xb9, .bytes = 3, .cycles = 4, .func = &opcode_lda, .mode = ABSY};
strcpy(optable[0xb9].mnemonic, "LDA");

	optable[0xa1] = (opcode_t){.code = 0xa1, .bytes = 2, .cycles = 6, .func = &opcode_lda, .mode = INDX};
strcpy(optable[0xa1].mnemonic, "LDA");

	optable[0xb1] = (opcode_t){.code = 0xb1, .bytes = 2, .cycles = 5, .func = &opcode_lda, .mode = INDY};
strcpy(optable[0xb1].mnemonic, "LDA");
	
	optable[0xa2] = (opcode_t){.code = 0xa2, .bytes = 2, .cycles = 2, .func = &opcode_ldx, .mode = IMM};
strcpy(optable[0xa2].mnemonic, "LDX");

	optable[0xa6] = (opcode_t){.code = 0xa6, .bytes = 2, .cycles = 3, .func = &opcode_ldx, .mode = ZP};
strcpy(optable[0xa6].mnemonic, "LDX");

	optable[0xb6] = (opcode_t){.code = 0xb6, .bytes = 2, .cycles = 4, .func = &opcode_ldx, .mode = ZPY};
strcpy(optable[0xb6].mnemonic, "LDX");

	optable[0xae] = (opcode_t){.code = 0xae, .bytes = 3, .cycles = 4, .func = &opcode_ldx, .mode = ABS};
strcpy(optable[0xae].mnemonic, "LDX");

	optable[0xbe] = (opcode_t){.code = 0xbe, .bytes = 3, .cycles = 4, .func = &opcode_ldx, .mode = ABSY};
strcpy(optable[0xbe].mnemonic, "LDX");

	optable[0xa0] = (opcode_t){.code = 0xa0, .bytes = 2, .cycles = 2, .func = &opcode_ldy, .mode = IMM};
strcpy(optable[0xa0].mnemonic, "LDY");

	optable[0xa4] = (opcode_t){.code = 0xa4, .bytes = 2, .cycles = 3, .func = &opcode_ldy, .mode = ZP};
strcpy(optable[0xa4].mnemonic, "LDY");

	optable[0xb4] = (opcode_t){.code = 0xb4, .bytes = 2, .cycles = 4, .func = &opcode_ldy, .mode = ZPX};
strcpy(optable[0xb4].mnemonic, "LDY");

	optable[0xac] = (opcode_t){.code = 0xac, .bytes = 3, .cycles = 4, .func = &opcode_ldy, .mode = ABS};
strcpy(optable[0xac].mnemonic, "LDY");

	optable[0xbc] = (opcode_t){.code = 0xbc, .bytes = 3, .cycles = 4, .func = &opcode_ldy, .mode = ABSY};
strcpy(optable[0xbc].mnemonic, "LDY");

	
	optable[0x85] = (opcode_t){.code = 0x85, .bytes = 2, .cycles = 3, .func = &opcode_sta, .mode = ZP};
strcpy(optable[0x85].mnemonic, "STA");

	optable[0x95] = (opcode_t){.code = 0x95, .bytes = 2, .cycles = 4, .func = &opcode_sta, .mode = ZPX};
strcpy(optable[0x95].mnemonic, "STA");

	optable[0x8d] = (opcode_t){.code = 0x8d, .bytes = 3, .cycles = 4, .func = &opcode_sta, .mode = ABS};
strcpy(optable[0x8d].mnemonic, "STA");

	optable[0x9d] = (opcode_t){.code = 0x9d, .bytes = 3, .cycles = 5, .func = &opcode_sta, .mode = ABSX};
strcpy(optable[0x9d].mnemonic, "STA");

	optable[0x99] = (opcode_t){.code = 0x99, .bytes = 3, .cycles = 5, .func = &opcode_sta, .mode = ABSY};
strcpy(optable[0x99].mnemonic, "STA");

	optable[0x81] = (opcode_t){.code = 0x81, .bytes = 2, .cycles = 6, .func = &opcode_sta, .mode = INDX};
strcpy(optable[0x81].mnemonic, "STA");

	optable[0x91] = (opcode_t){.code = 0x91, .bytes = 2, .cycles = 6, .func = &opcode_sta, .mode = INDY};
strcpy(optable[0x91].mnemonic, "STA");

	optable[0x86] = (opcode_t){.code = 0x86, .bytes = 2, .cycles = 3, .func = &opcode_stx, .mode = ZP};
strcpy(optable[0x86].mnemonic, "STX");

	optable[0x96] = (opcode_t){.code = 0x96, .bytes = 2, .cycles = 4, .func = &opcode_stx, .mode = ZPY};
strcpy(optable[0x96].mnemonic, "STX");

	optable[0x8e] = (opcode_t){.code = 0x8e, .bytes = 3, .cycles = 4, .func = &opcode_stx, .mode = ABS};
strcpy(optable[0x8e].mnemonic, "STX");

	optable[0x84] = (opcode_t){.code = 0x84, .bytes = 2, .cycles = 3, .func = &opcode_sty, .mode = ZP};
strcpy(optable[0x84].mnemonic, "STY");

	optable[0x94] = (opcode_t){.code = 0x94, .bytes = 2, .cycles = 4, .func = &opcode_sty, .mode = ZPX};
strcpy(optable[0x94].mnemonic, "STY");

	optable[0x8c] = (opcode_t){.code = 0x8c, .bytes = 3, .cycles = 4, .func = &opcode_sty, .mode = ABS};
strcpy(optable[0x8c].mnemonic, "STY");


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
		else if (opcode.func == NULL) {
			break;
		}
		else if (opcode.func != NULL) {
			opcode.func(cpu, opcode.mode);
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
