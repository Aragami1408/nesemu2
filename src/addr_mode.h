#ifndef ADDR_MODE_H
#define ADDR_MODE_H

enum addressing_mode_t {
	// Immediate
	IMM,

	// Zero Page
	ZP,
	ZPX,
	ZPY,

	// Absolute
	ABS,
	ABSX,
	ABSY,

	// Indirect
	INDX,
	INDY,

	// None Addressing
	NONE
};

#endif
