#include <iostream>
#include <gtest/gtest.h>

extern "C" {
	#include "../src/cpu.h"
}

TEST(opcode_test, test_load_store) {
	
	cpu_t *cpu = cpu_init();
	u8 program1[] = {
		0xa9, 0x20, // lda #$20
		0xa2, 0x30, // ldx #$30
		0xa0, 0x40, // ldy #$40
		0x85, 0x20, // sta $20
		0x86, 0x21, // stx $21
		0x84, 0x22, // sty $22
		0x00
	};
	cpu_load_and_run(cpu, program1, 13);

	EXPECT_EQ(cpu->a, 0x20);
	EXPECT_EQ(cpu->x, 0x30);
	EXPECT_EQ(cpu->y, 0x40);
	
	EXPECT_EQ(cpu->memory[0x20], 0x20);
	EXPECT_EQ(cpu->memory[0x21], 0x30);
	EXPECT_EQ(cpu->memory[0x22], 0x40);

	cpu_free(cpu);
}

TEST(opcode_test, test_register_transfers) {	
	cpu_t *cpu = cpu_init();

	u8 program1[] = {
		0xa9, 0xde, // lda #$de
		0xaa,		// tax
		0x00
	};	

	u8 program2[] = {
		0xa9, 0xdf, // lda #$df
		0xa8,		// tay
		0x00
	};

	u8 program3[] = {
		0xa2, 0xad,
		0x8a,
		0x00
	};

	u8 program4[] = {
		0xa0, 0xab,
		0x98,
		0x00
	};

	cpu_load_and_run(cpu, program1, 4);
	EXPECT_EQ(cpu->x, 0xde);
	cpu_load_and_run(cpu, program2, 4);
	EXPECT_EQ(cpu->y, 0xdf);
	cpu_load_and_run(cpu, program3, 4);
	EXPECT_EQ(cpu->a, 0xad);
	cpu_load_and_run(cpu, program4, 4);
	EXPECT_EQ(cpu->a, 0xab);

	cpu_free(cpu);
}

TEST(opcode_test, test_logical_bitwise) {
	cpu_t *cpu = cpu_init();

	cpu_free(cpu);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

