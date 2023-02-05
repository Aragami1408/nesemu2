#include "cpu.h"
#include "types.h"

static u16 get_operand_address(cpu_t *cpu, enum addressing_mode_t am);
static void set_flag(cpu_t *cpu, bool v, u8 flag);

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
			return (u16) pos + cpu->x;
		} break;
		case ZPY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);
			return (u16) pos + cpu->y;
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
			u8 hi = cpu_mem_read(cpu, (u16) ptr + 1);
			return ((u16) hi) << 8 | ((u16) lo);
		} break;

		case INDY: {
			u8 pos = cpu_mem_read(cpu, cpu->pc);		

			u8 lo = cpu_mem_read(cpu, (u16) pos);
			u8 hi = cpu_mem_read(cpu, (u16) pos + 1);
			u16 deref_pos = ((u16) hi) << 8 | ((u16) lo);
			return deref_pos + (u16) cpu->y;

		} break;

		default: {
			printf("Addressing Mode not supported\n");
		} break;
	}

	return -1;
}


void opcode_and(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	cpu->a &= value;

	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_eor(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	cpu->a ^= value;

	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_ora(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);
	cpu->a |= value;

	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_bit(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	u8 result = cpu->a & value;

	set_flag(cpu, result == 0x00, SF_ZERO);
	set_flag(cpu, (value & 0x40) != 0x0, SF_OVERFLOW);
	set_flag(cpu, (value & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_dec(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);	

	cpu_mem_write(cpu, addr, value - 1);

	set_flag(cpu, value - 1 == 0x00, SF_ZERO);
	set_flag(cpu, ((value - 1) & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_dex(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x--;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->x & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_dey(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y--;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, cpu->y & 0x80, SF_NEGATIVE);
}

void opcode_lda(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->a = value;
	set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_ldx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->x = value;
	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->x & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_ldy(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u16 addr = get_operand_address(cpu, addr_mode);
	u8 value = cpu_mem_read(cpu, addr);

	cpu->y = value;
	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->y & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_tax(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x = cpu->a;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->x & 0x80) != 0x0, SF_NEGATIVE);
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
	cpu->y = cpu->a;

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

	cpu_mem_write(cpu, addr, value + 1);

	set_flag(cpu, value + 1 == 0x00, SF_ZERO);
	set_flag(cpu, ((value + 1) & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_inx(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->x++;

	set_flag(cpu, cpu->x == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->x & 0x80) != 0x0, SF_NEGATIVE);
}

void opcode_iny(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->y++;

	set_flag(cpu, cpu->y == 0x00, SF_ZERO);
	set_flag(cpu, (cpu->y & 0x80) != 0x0, SF_NEGATIVE);

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

		set_flag(cpu, (cpu->a & 0x40) != 0x0, SF_CARRY);
		u8 result = cpu->a << 1;
		cpu->a = result;
		set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 value = cpu_mem_read(cpu, addr);

		set_flag(cpu, (value & 0x40) != 0x0, SF_CARRY);
		u8 result = value << 1;
		cpu_mem_write(cpu, addr, result);

		set_flag(cpu, (result & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, result == 0x00, SF_ZERO);
	}
}

void opcode_lsr(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {

		set_flag(cpu, (cpu->a & 0x40) != 0x0, SF_CARRY);
		u8 result = cpu->a >> 1;
		cpu->a = result;
		set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 value = cpu_mem_read(cpu, addr);

		set_flag(cpu, (value & 0x40) != 0x0, SF_CARRY);
		u8 result = value >> 1;
		cpu_mem_write(cpu, addr, result);

		set_flag(cpu, (result & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, result == 0x00, SF_ZERO);
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
		cpu->a = data;
		set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, cpu->a == 0x00, SF_ZERO);
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
		set_flag(cpu, (data & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, data == 0x00, SF_ZERO);	
	}
}

void opcode_ror(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	if (addr_mode == ACC) {
		u8 data = cpu->a;
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;

		set_flag(cpu, (data & 1) == 1, SF_CARRY);
		data >>= 1;
		if (old_carry) {
			data |= 0x80;
		}
		cpu->a = data;
		set_flag(cpu, (cpu->a & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, cpu->a == 0x00, SF_ZERO);
	}
	else {
		u16 addr = get_operand_address(cpu, addr_mode);
		u8 data = cpu_mem_read(cpu, addr);
		bool old_carry = (cpu->sr & SF_CARRY) != 0x0;
		set_flag(cpu, (data & 1) == 1, SF_CARRY);
		data >>= 1;
		if (old_carry) {
			data |= 0x80;
		}
		cpu_mem_write(cpu, addr, data);
		set_flag(cpu, (data & 0x80) != 0x0, SF_NEGATIVE);
		set_flag(cpu, data == 0x00, SF_ZERO);	
	}
	
}

void opcode_pha(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu_stack_push(cpu, cpu->a);
}

void opcode_php(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u8 flags = cpu->sr;	
	flags |= SF_BREAK | SF_BREAK2;
	cpu_stack_push(cpu, flags);
}

void opcode_pla(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	u8 data = cpu_stack_pop(cpu);
	cpu->a = data;
}

void opcode_plp(cpu_t *cpu, enum addressing_mode_t addr_mode) {
	cpu->sr = cpu_stack_pop(cpu);
	cpu->sr &= ~SF_BREAK;	
	cpu->sr |= SF_BREAK2;	
}

