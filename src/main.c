#include "cpu.h"
#include "logger.h"

#include <stdio.h>


int main() {
	cpu_t *cpu = cpu_init();
	if (cpu == NULL) {
		return -1;
	}
	u8 program[] = {0xa9, 0x20, 0xa2, 0x30, 0xa0, 0x40, 0x85, 0x20, 0x86, 0x21, 0x84, 0x22, 0x00};
	size_t program_size = sizeof(program)/sizeof(program[0]);
	cpu_load_and_run(cpu, program, program_size);
	cpu_regdump(cpu);
	printf("0x%hhx\n", cpu->memory[0x20]);
	printf("0x%hhx\n", cpu->memory[0x21]);
	printf("0x%hhx\n", cpu->memory[0x22]);
	cpu_free(cpu);

	
	
	return 0;
}
