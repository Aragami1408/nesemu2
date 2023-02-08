#include <gtest/gtest.h>

extern "C" {
	#include "../src/cpu.h"
}

TEST(opcode_test, test_lda_immediate_load_data) {
	cpu_t *cpu = cpu_init();	
	u8 program[] = {0xa9, 0x05, 0x00};
	cpu_load_and_run(cpu, program, 3);
	EXPECT_EQ(cpu->a, 5);
	EXPECT_EQ(cpu->sr & 0b00000010, 0b00);
	EXPECT_EQ(cpu->sr & 0b10000000, 0b00);
	cpu_free(cpu);
}

TEST(opcode_test, test_tax_move_a_to_x) {
	cpu_t *cpu = cpu_init();	
	cpu->a = 10;
	u8 program[] = {0xaa, 0x00};
	cpu_load_and_run(cpu, program, 2);
	EXPECT_EQ(cpu->x, 10);
	cpu_free(cpu);
}

TEST(opcode_test, test_5_ops_working_together) {
	cpu_t *cpu = cpu_init();	
	u8 program[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
	cpu_load_and_run(cpu, program, 5);
	EXPECT_EQ(cpu->x, 0xc1);
	cpu_free(cpu);
}

TEST(opcode_test, test_inx_overflow) {
	cpu_t *cpu = cpu_init();	
	cpu->x = 0xff;
	u8 program[] = {0xe8, 0xe8, 0x00};
	cpu_load_and_run(cpu, program, 3);
	EXPECT_EQ(cpu->x, 1);
	cpu_free(cpu);
	
}

TEST(opcode_test, test_lda_from_memory) {
	cpu_t *cpu = cpu_init();	
	cpu_mem_write(cpu, 0x10, 0x55);
	u8 program[] = {0xa5, 0x10, 0x00};
	cpu_load_and_run(cpu, program, 3);
	EXPECT_EQ(cpu->a, 0x55);
	cpu_free(cpu);
	
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
