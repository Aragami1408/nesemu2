#include "cpu.h"
#include "types.h"

#include <stdio.h>

static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am);

static void set_flag(cpu_t *cpu, bool v, u8 flag);
static void update_zero_and_negative_flags(cpu_t *cpu, u8 result);
static void update_negative_flags(cpu_t *cpu, u8 result);
static void set_carry_flag(cpu_t *cpu);
static void clear_carry_flag(cpu_t *cpu);

static void set_reg_a(cpu_t *cpu, u8 value);
static void add_to_reg_a(cpu_t *cpu, u8 data);

static void compare(cpu_t *cpu, enum addressing_mode_t am, u8 compare_with);
static void branch(cpu_t *cpu, bool condition);


static void set_flag(cpu_t *cpu, bool v, u8 flag) {
	if (v) cpu->sr |= flag;
	else cpu->sr &= ~flag;
}

static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am) {
	switch (am) {
		case IMM:
			return cpu->pc;	
			break;

		case ZP:
			return (u16) cpu_mem_read(cpu, cpu->pc);
			break;
		case ZPX: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			u8 addr = (pos + cpu->x);
			return (u16) addr;
		} break;
		case ZPY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			u8 addr = (u16) (pos + cpu->y);
			return (u16) addr;
		} break;

		case ABS:
			return cpu_mem_read_u16(cpu, cpu->pc);
			break;
		case ABSX: {
			u16 pos = cpu_mem_read_u16(cpu, cpu->pc);
			return pos + (u16) cpu->x;
		} break;
		case ABSY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			return pos + (u16) cpu->y;
		} break;

		case INDX: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);		

			u8 ptr = pos + cpu->x;
			u8 lo = cpu_mem_read(cpu, (u16) ptr);
			u8 hi = cpu_mem_read(cpu, (u16) (ptr + 1));
			return ((u16) hi) << 8 | ((u16) lo);
		} break;

		case INDY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);		

			u8 lo = cpu_mem_read(cpu, (u16) pos);
			u8 hi = cpu_mem_read(cpu, (u16) (pos + 1));
			u16 deref_pos = ((u16) hi) << 8 | ((u16) lo);
			return deref_pos + (u16) cpu->y;
		} break;

		case ACC:
			break;
		
		case IND:
			break;

		case NONE:
			break;

		default: {
			printf("Addressing Mode not supported\n");
		} break;
	}

	return (u16) -1;
}

static void update_negative_flags(cpu_t *cpu, u8 result) {
	set_flag(cpu, (result >> 7) == 1, SF_NEGATIVE);		
}


static void update_zero_and_negative_flags(cpu_t *cpu, u8 result) {
	set_flag(cpu, result == 0, SF_ZERO);
	set_flag(cpu, (result >> 7) == 1, SF_NEGATIVE);
}


static void set_carry_flag(cpu_t *cpu) {
	cpu->sr |= SF_CARRY;
}

static void clear_carry_flag(cpu_t *cpu) {
	cpu->sr &= (u8) (~SF_CARRY);
}

static void set_reg_a(cpu_t *cpu, u8 value) {
	cpu->a = value;
	update_zero_and_negative_flags(cpu, cpu->a);
}


static void add_to_reg_a(cpu_t *cpu, u8 data) {
	u16 sum = (u16) (cpu->a + data);
	sum += (cpu->sr & SF_CARRY) ? 1 : 0;

	bool carry = sum > 0xff;
	set_flag(cpu, carry, SF_CARRY);
	u8 result = (u8) sum;
	set_flag(cpu, ((data ^ result) & (result ^ cpu->a) & 0x80) != 0, SF_OVERFLOW);
	set_reg_a(cpu, result);
}

static void compare(cpu_t *cpu, enum addressing_mode_t am, u8 compare_with) {
	u16 addr = get_operand_address(cpu, am);
	u8 data = cpu_mem_read(cpu, addr);
	
	set_flag(cpu, data <= compare_with, SF_CARRY);
	update_zero_and_negative_flags(cpu, compare_with - data);
}

static void branch(cpu_t *cpu, bool condition) {
	if (condition) {
		i8 jump = (i8) cpu_mem_read(cpu, cpu->pc);
		u16 jump_addr = (u16) (cpu->pc + 1 + jump);

		cpu->pc = jump_addr;
	}
}


void opcode_and(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	set_reg_a(cpu, value & cpu->a);
}

void opcode_eor(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	set_reg_a(cpu, value ^ cpu->a);
}

void opcode_ora(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	set_reg_a(cpu, value | cpu->a);
}

void opcode_bit(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	u8 result = cpu->a & value;

	set_flag(cpu, result == 0x00, SF_ZERO);
	set_flag(cpu, (result & 0b10000000) > 0, SF_NEGATIVE);
	set_flag(cpu, (result & 0b01000000) > 0, SF_OVERFLOW);
}

void opcode_dec(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);	
	value--;
	cpu_mem_write(cpu, addr, value);
	update_zero_and_negative_flags(cpu, value);
}

void opcode_dex(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x--;
	update_zero_and_negative_flags(cpu, cpu->x);
}

void opcode_dey(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y--;
	update_zero_and_negative_flags(cpu, cpu->y);
}

void opcode_lda(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	set_reg_a(cpu, value);
}

void opcode_ldx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->x = value;
	update_zero_and_negative_flags(cpu, cpu->x);
}

void opcode_ldy(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->y = value;
	update_zero_and_negative_flags(cpu, cpu->y);
}

void opcode_tax(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x = cpu->a;
	update_zero_and_negative_flags(cpu, cpu->x);
}

void opcode_tay(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y = cpu->a;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->y & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_txa(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->a = cpu->x;

	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_tya(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->a = cpu->y;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->y & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_tsx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x = cpu->sp;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->x & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_txs(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sp = cpu->x;
}

void opcode_inc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);	
	value++;
	cpu_mem_write(cpu, addr, value);
	update_zero_and_negative_flags(cpu, value);
}

void opcode_inx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x++;
	update_zero_and_negative_flags(cpu, cpu->x);	
}

void opcode_iny(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y++;
	update_zero_and_negative_flags(cpu, cpu->x);	
}

void opcode_sta(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->a);
}

void opcode_stx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->x);
	
}

void opcode_sty(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	cpu_mem_write(cpu, addr, cpu->y);

}

void opcode_asl(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {
		u8 data = cpu->a;	
		set_flag(cpu, (data >> 7) == 1, SF_CARRY);
		data <<= 1;
		set_reg_a(cpu, data);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 value = cpu_mem_read(cpu, addr);

		set_flag(cpu, (value >> 7) == 1, SF_CARRY);
		value <<= 1;
		cpu_mem_write(cpu, addr, value);
		update_zero_and_negative_flags(cpu, value);
	}
}

void opcode_lsr(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {
		u8 data = cpu->a;
		set_flag(cpu, (data & 1) == 1, SF_CARRY);
		data >>= 1;
		set_reg_a(cpu, data);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 value = cpu_mem_read(cpu, addr);

		set_flag(cpu, (value & 1) == 1, SF_CARRY);
		value >>= 1;
		cpu_mem_write(cpu, addr, value);
		update_zero_and_negative_flags(cpu, value);
	}

}

void opcode_rol(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {
		u8 data = cpu->a;
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;

		set_flag(cpu, (data >> 7) == 1, SF_CARRY);
		data <<= 1;
		if (old_carry) {
			data |= 1;
		}
		set_reg_a(cpu, data);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 data = cpu_mem_read(cpu, addr);
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;
		set_flag(cpu, (data >> 7) == 1, SF_CARRY);
		data <<= 1;
		if (old_carry) {
			data |= 1;
		}
		cpu_mem_write(cpu, addr, data);
		update_negative_flags(cpu, data);
	}
}

void opcode_ror(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {
		u8 data = cpu->a;
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;

		set_flag(cpu, (data & 1) == 1, SF_CARRY);
		data >>= 1;
		if (old_carry) {
			data |= 0b1000000;
		}
		set_reg_a(cpu, data);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 data = cpu_mem_read(cpu, addr);
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;
		set_flag(cpu, (data & 1) == 1, SF_CARRY);
		data >>= 1;
		if (old_carry) {
			data |= 0b1000000;
		}
		cpu_mem_write(cpu, addr, data);
		update_negative_flags(cpu, data);
	}
	
}

void opcode_pha(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu_stack_push(cpu, cpu->a);
}

void opcode_php(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u8 flags = cpu->sr;	
	flags |= SF_BREAK;
	flags |= SF_BREAK2;
	cpu_stack_push(cpu, flags);
}

void opcode_pla(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u8 data = cpu_stack_pop(cpu);
	set_reg_a(cpu, data);
}

void opcode_plp(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr = cpu_stack_pop(cpu);
	cpu->sr &= (u8) ~(SF_BREAK);	
	cpu->sr |= SF_BREAK2;	
}

void opcode_jmp(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ABS) {
		u16 mem_address = cpu_mem_read_u16(cpu, cpu->pc);
		cpu->pc = mem_address;
	}
	else if (addr_mode == IND) {
		u16 mem_address = cpu_mem_read_u16(cpu, cpu->pc);
		
		u16 indirect_ref;
		if ((mem_address & 0x00ff) == 0x00ff) {
			u8 lo = cpu_mem_read(cpu, mem_address);
			u8 hi = cpu_mem_read(cpu, mem_address & 0xff00);
			indirect_ref = hi << 8 | lo;
		}
		else {
			indirect_ref = cpu_mem_read_u16(cpu, mem_address);
		}
		
		cpu->pc = indirect_ref;
	}
	else {

	}
}

void opcode_jsr(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu_stack_push_u16(cpu, cpu->pc + 2 - 1);
	u16 target_address = cpu_mem_read_u16(cpu, cpu->pc);
	cpu->pc = target_address;
}

void opcode_rts(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->pc = cpu_stack_pop_u16(cpu) + 1;
}

void opcode_adc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	add_to_reg_a(cpu, value);
}

void opcode_sbc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	add_to_reg_a(cpu,(u8) ((-((i8) value))-1));	
}

void opcode_cmp(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	compare(cpu, addr_mode, cpu->a);	
}

void opcode_cpx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	compare(cpu, addr_mode, cpu->x);	
}

void opcode_cpy(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	compare(cpu, addr_mode, cpu->y);	
}

void opcode_bcc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_CARRY) == 0x00);
}
void opcode_bcs(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	
	branch(cpu, (cpu->sr & SF_CARRY) != 0x00);
}
void opcode_beq(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_ZERO) != 0x00);
}
void opcode_bmi(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_NEGATIVE) != 0x00);
}
void opcode_bne(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_ZERO) == 0x00);
}
void opcode_bpl(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_NEGATIVE) == 0x00);
}
void opcode_bvc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_OVERFLOW) == 0x00);
}
void opcode_bvs(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	branch(cpu, (cpu->sr & SF_OVERFLOW) != 0x00);
}

void opcode_clc(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	clear_carry_flag(cpu);
}
void opcode_cld(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr &= (u8) ~(SF_DECIMAL);
}
void opcode_cli(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr &= (u8) ~(SF_INTERRUPT);
}
void opcode_clv(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr &= (u8) ~(SF_OVERFLOW);
}
void opcode_sec(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	set_carry_flag(cpu);
}
void opcode_sed(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr |= SF_DECIMAL;
}
void opcode_sei(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr |= SF_INTERRUPT;
}

void opcode_rti(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr = cpu_stack_pop(cpu);
	cpu->sr &= (u8) ~(SF_BREAK);
	cpu->sr |= SF_BREAK2;


	cpu->pc = cpu_stack_pop_u16(cpu);
}
