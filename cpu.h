#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "addr_mode.h"

#include "logger.h"

#define MEMORY_MAX 0xFFFF

typedef struct cpu cpu_t;
typedef struct opcode opcode_t;

// STATUS FLAGS
#define SF_NEGATIVE 	 1 << 7
#define SF_OVERFLOW 	 1 << 6
#define SF_BREAK 		 1 << 4
#define SF_DECIMAL 	 	 1 << 3
#define SF_INTERRUPT 	 1 << 2
#define SF_ZERO 		 1 << 1
#define SF_CARRY 		 1 << 0

cpu_t *cpu_init();

u8 cpu_mem_read(cpu_t *cpu, u16 addr);
void cpu_mem_write(cpu_t *cpu, u16 addr, u8 data);

u16 cpu_mem_read_u16(cpu_t *cpu, u16 pos);
void cpu_mem_write_u16(cpu_t *cpu, u16 pos, u16 data);

void cpu_reset(cpu_t *cpu);

void cpu_load(cpu_t *cpu, u8 *program, int size);
void cpu_run(cpu_t *cpu);
void cpu_load_and_run(cpu_t *cpu, u8 *program, int size);


void cpu_regdump(cpu_t *cpu);

void cpu_free(cpu_t *cpu);


struct cpu {
	u8 a; 			// Register A
	u8 x;			// Register X
	u8 y;			// Register Y

	u16 pc;			// Program Counter
	u16 sp; 		// Stack Pointer					
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

void opcode_and(cpu_t *cpu, enum addressing_mode_t addr_mode);

void opcode_dex(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_dey(cpu_t *cpu, enum addressing_mode_t addr_mode);

void opcode_inx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_iny(cpu_t *cpu, enum addressing_mode_t addr_mode);

void opcode_lda(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ldx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_ldy(cpu_t *cpu, enum addressing_mode_t addr_mode);

void opcode_sta(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_stx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_sty(cpu_t *cpu, enum addressing_mode_t addr_mode);

void opcode_tax(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_tay(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_txa(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_tya(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_tsx(cpu_t *cpu, enum addressing_mode_t addr_mode);
void opcode_txs(cpu_t *cpu, enum addressing_mode_t addr_mode);

#endif
