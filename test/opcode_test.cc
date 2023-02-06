#include <iostream>
#include <gtest/gtest.h>

#include "../src/cpu.h"

TEST(opcode_test, test_load_store) {
	
	cpu_t *cpu = cpu_init();
	u8 program[] = {0xa9, 0x20, 0xa2, 0x30, 0xa0, 0x40, 0x85, 0x20, 0x86, 0x21, 0x84, 0x22, 0x00};
	size_t program_size = sizeof(program)/sizeof(program[0]);
	cpu_load_and_run(cpu, program, program_size);

	EXPECT_EQ(cpu->a, 0x20);
	EXPECT_EQ(cpu->x, 0x30);
	EXPECT_EQ(cpu->y, 0x40);

	cpu_free(cpu);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

