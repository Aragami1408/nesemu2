#include <gtest/gtest.h>

extern "C" {
	#include "../src/cpu.h"
}

class CPU6502Test : public ::testing::Test {
public:
	cpu_t *cpu;

	void SetUp() override {
		cpu = cpu_init();	
	}

	void TearDown() override {
		cpu_free(cpu);
	}
};

static void verify_unmodified_flags(const cpu_t *cpu, const cpu_t *cpu_copy) {
	EXPECT_EQ(cpu->sr & SF_CARRY, cpu_copy->sr & SF_CARRY);
	EXPECT_EQ(cpu->sr & SF_CARRY, cpu_copy->sr & SF_CARRY);
	EXPECT_EQ(cpu->sr & SF_CARRY, cpu_copy->sr & SF_CARRY);
	EXPECT_EQ(cpu->sr & SF_CARRY, cpu_copy->sr & SF_CARRY);
	EXPECT_EQ(cpu->sr & SF_CARRY, cpu_copy->sr & SF_CARRY);
}

TEST_F(CPU6502Test, lda_imm_test) {
	u8 program[] = {0xa9, 0x84, 0x00};

	cpu_t *cpu_copy = cpu;
	cpu_load_and_run(cpu, program, 3);

	EXPECT_EQ(cpu->a, 0x84);
	EXPECT_TRUE(cpu->sr & SF_NEGATIVE);
	EXPECT_FALSE(cpu->sr & SF_ZERO);
	verify_unmodified_flags(cpu, cpu_copy);
}

TEST_F(CPU6502Test, lda_zp_test) {
	u8 program[] = {0xa5, 0x42, 0x00};

	cpu_t *cpu_copy = cpu;
	cpu_mem_write(cpu, 0x0042, 0x37);
	cpu_load_and_run(cpu, program, 3);

	EXPECT_EQ(cpu->a, 0x37);
	EXPECT_FALSE(cpu->sr & SF_ZERO);
	EXPECT_FALSE(cpu->sr & SF_NEGATIVE);
	verify_unmodified_flags(cpu, cpu_copy);
}

TEST_F(CPU6502Test, lda_zpx_test) {
	cpu->x = 5;
	u8 program1[] = {0xb5, 0x42, 0x00};
	cpu_mem_write(cpu, 0x47, 0x37);

	cpu_t *cpu_copy = cpu;
	cpu_load_and_run(cpu, program1, 3);

	EXPECT_EQ(cpu->a, 0x37);
	EXPECT_FALSE(cpu->sr & SF_ZERO);
	EXPECT_FALSE(cpu->sr & SF_NEGATIVE);
	verify_unmodified_flags(cpu, cpu_copy);
}

TEST_F(CPU6502Test, lda_zpx_test_if_wrap_test) {
	cpu_reset(cpu);
	// when it wraps
	cpu->x = 0xff;
	u8 program2[] = {0xb5, 0x80, 0x00};
	cpu_mem_write(cpu, 0x7f, 0x37);

	cpu_t *cpu_copy = cpu;
	cpu_load_and_run(cpu, program2, 3);

	EXPECT_EQ(cpu->a, 0x37);
	EXPECT_FALSE(cpu->sr & SF_ZERO);
	EXPECT_FALSE(cpu->sr & SF_NEGATIVE);
	verify_unmodified_flags(cpu, cpu_copy);
}