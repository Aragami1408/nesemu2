#include "cpu.h"
#include "logger.h"

#include <stdio.h>


int main() {
	cpu_t *cpu = cpu_init();
	if (cpu == NULL) {
		return -1;
	}
	cpu_mem_write(cpu, 0x10, 0x55);
	u8 program[] = {0xa9, 0xf0, 0x85, 0x20, 0xa9, 0x30, 0x24, 0x20};
	size_t program_size = sizeof(program)/sizeof(program[0]);
	cpu_load_and_run(cpu, program, program_size);
	cpu_regdump(cpu);
	cpu_free(cpu);

	
	
	return 0;
}
