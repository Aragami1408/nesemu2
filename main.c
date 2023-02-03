#include "cpu.h"

#include <stdio.h>


int main() {
	cpu_t *cpu = cpu_init();
	cpu_mem_write(cpu, 0x10, 0x55);
	u8 program[] = {0xa5, 0x10, 0x00};
	size_t program_size = sizeof(program)/sizeof(program[0]);
	cpu_load_and_run(cpu, program, program_size);

	cpu_regdump(cpu);
	cpu_free(cpu);

	
	
	return 0;
}
