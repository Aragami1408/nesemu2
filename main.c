#include "cpu.h"

#include <stdio.h>


int main() {
	cpu_t *cpu = cpu_init();
	u8 test[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};

	cpu_load_and_run(cpu, test, 5);

	cpu_regdump(cpu);
	
	return 0;
}
