#include "cpu.h"

#include <stdio.h>


int main() {
	cpu_t *cpu = cpu_init();
	cpu_mem_write(cpu, 0x10, 0x55);

	u8 program[] = {0xA5, 0x10, 0x00};
	cpu_load_and_run(cpu, program, 3);

	cpu_regdump(cpu);

	cpu_free(cpu);
	
	return 0;
}
