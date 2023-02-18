#ifndef CPU_H
#define CPU_H

#include "types.h"

#include <stdbool.h>

#define MEMORY_MAX 0xFFFF
#define STACK 0x100
#define STACK_RESET 0xfd;

typedef struct cpu cpu_t;
typedef struct opcode opcode_t;

// STATUS FLAGS

//	7 6 5 4 3 2 1 0
//	N V _ B D I Z C
//  | |   | | | | +--- Carry Flag
//  | |   | | | +----- Zero Flag
//  | |   | | +------- Interrupt Disable
//  | |   | +--------- Decimal Mode (not used on NES)
//  | |   +----------- Break Command
//  | +--------------- Overflow Flag
//  +----------------- Negative Flag

#define SF_NEGATIVE 	 1u << 7
#define SF_OVERFLOW 	 1u << 6
#define SF_BREAK2		 1u << 5
#define SF_BREAK 		 1u << 4
#define SF_DECIMAL 	 	 1u << 3
#define SF_INTERRUPT 	 1u << 2
#define SF_ZERO 		 1u << 1
#define SF_CARRY 		 1u << 0

enum addressing_mode_t {
	// Accumulator
	ACC,
	// Immediate
	IMM,
	// Zero Page
	ZP,
	ZPX,
	ZPY,
	// Absolute
	ABS,
	ABSX,
	ABSY,
	// Indirect
	IND,
	INDX,
	INDY,
	// None Addressing
	NONE
};

cpu_t *cpu_init(void);

u8 cpu_mem_read(cpu_t *cpu, u16 addr);
void cpu_mem_write(cpu_t *cpu, u16 addr, u8 data);

u16 cpu_mem_read_u16(cpu_t *cpu, u16 pos);
void cpu_mem_write_u16(cpu_t *cpu, u16 pos, u16 data);

void cpu_stack_push(cpu_t *cpu, u8 data);
u8 cpu_stack_pop(cpu_t *cpu);

void cpu_stack_push_u16(cpu_t *cpu, u16 data);
u16 cpu_stack_pop_u16(cpu_t *cpu);

void cpu_compare(cpu_t *cpu, enum addressing_mode_t addr_mode, u8 cmp);

void cpu_reset(cpu_t *cpu);

void cpu_load(cpu_t *cpu, u8 *program, int size);
void cpu_run(cpu_t *cpu);
void cpu_run_with_callback(cpu_t *cpu, void (*callback)(cpu_t *cpu));
void cpu_load_and_run(cpu_t *cpu, u8 *program, int size);

void cpu_free(cpu_t *cpu);

struct cpu {
	u8 a; 			// Register A
	u8 x;			// Register X
	u8 y;			// Register Y

	u16 pc;			// Program Counter
	u8 sp; 			// Stack Pointer					
	u8 sr; 			// Status Register

	u8 memory[MEMORY_MAX];
};


typedef void (*opcode_func)(cpu_t *, enum addressing_mode_t);

struct opcode {
	u8 code;
	char mnemonic[4];
	int bytes;
	int cycles;
	opcode_func func;
	enum addressing_mode_t mode;
};

// Load/Store Operations
void opcode_lda(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ldx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ldy(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sta(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_stx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sty(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Register Transfers
void opcode_tax(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_tay(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_txa(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_tya(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Stack Operations
void opcode_tsx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_txs(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_pha(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_php(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_pla(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_plp(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Logical
void opcode_and(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_eor(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ora(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bit(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Arithmetic
void opcode_adc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sbc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_cmp(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_cpx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_cpy(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Increments & Decrements
void opcode_inc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_inx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_iny(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_dec(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_dex(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_dey(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Shifts
void opcode_asl(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_lsr(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_rol(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ror(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Jumps & Calls
void opcode_jmp(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_jsr(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_rts(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Branches
void opcode_bcc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bcs(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_beq(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bmi(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bne(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bpl(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bvc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_bvs(cpu_t *cpu, enum addressing_mode_t addr_mode);

// Status Flag Changes
void opcode_clc(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_cld(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_cli(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_clv(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sec(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sed(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sei(cpu_t *cpu, enum addressing_mode_t addr_mode);

// System Functions
void opcode_rti(cpu_t *cpu, enum addressing_mode_t addr_mode);

#endif
