/*
 * libcpu: x86_decode.h
 *
 * instruction decoding
 */

#ifndef X86_DECODE_H
#define X86_DECODE_H

#include "libcpu.h"

#include <stdint.h>

enum x86_operand_type {
	OP_IMM,
	OP_MEM,
	OP_MEM_DISP,
	OP_REG,
	OP_SEG_REG,
	OP_REL,
};

enum x86_seg_override {
	NO_OVERRIDE,
	ES_OVERRIDE,
	CS_OVERRIDE,
	SS_OVERRIDE,
	DS_OVERRIDE,
};

enum x86_rep_prefix {
	NO_PREFIX,
	REPNZ_PREFIX,
	REPZ_PREFIX,
};

struct x86_operand {
	enum x86_operand_type	type;
	uint8_t			reg;
	int32_t			disp;		/* address displacement can be negative */
	union {
		uint32_t		imm;
		int32_t			rel;
	};
};

enum x86_instr_flags {
	MOD_RM			= (1U << 8),
	DIR_REVERSED		= (1U << 9),

	/* Operand sizes */
	WIDTH_BYTE		= (1U << 10),	/* 8 bits */
	WIDTH_FULL		= (1U << 11),	/* 16 bits or 32 bits */
	WIDTH_MASK		= WIDTH_BYTE|WIDTH_FULL,

	/* Source operand */
	SRC_NONE		= (1U << 12),

	SRC_IMM			= (1U << 13),
	SRC_IMM8		= (1U << 14),
	IMM_MASK		= SRC_IMM|SRC_IMM8,

	SRC_REL			= (1U << 15),
	REL_MASK		= SRC_REL,

	SRC_REG			= (1U << 16),
	SRC_SEG_REG		= (1U << 17),
	SRC_ACC			= (1U << 18),
	SRC_MEM			= (1U << 19),
	SRC_MOFFSET		= (1U << 20),
	SRC_MEM_DISP_BYTE	= (1U << 21),
	SRC_MEM_DISP_FULL	= (1U << 22),
	SRC_MASK		= SRC_NONE|IMM_MASK|REL_MASK|SRC_REG|SRC_SEG_REG|SRC_ACC|SRC_MEM|SRC_MOFFSET|SRC_MEM_DISP_BYTE|SRC_MEM_DISP_FULL,

	/* Destination operand */
	DST_NONE		= (1U << 23),
	DST_REG			= (1U << 24),
	DST_ACC			= (1U << 25),	/* AL/AX */
	DST_MEM			= (1U << 26),
	DST_MOFFSET		= (1U << 27),
	DST_MEM_DISP_BYTE	= (1U << 28),	/* 8 bits */
	DST_MEM_DISP_FULL	= (1U << 29),	/* 16 bits or 32 bits */
	DST_MASK		= DST_NONE|DST_REG|DST_ACC|DST_MOFFSET|DST_MEM|DST_MEM_DISP_BYTE|DST_MEM_DISP_FULL,

	MEM_DISP_MASK		= SRC_MEM|SRC_MEM_DISP_BYTE|SRC_MEM_DISP_FULL|DST_MEM|DST_MEM_DISP_BYTE|DST_MEM_DISP_FULL,

	MOFFSET_MASK		= SRC_MOFFSET|DST_MOFFSET,

	GROUP_2			= (1U << 30),

	GROUP_MASK		= GROUP_2,
};

/*
 *	Addressing modes.
 */
enum x86_addmode {
	ADDMODE_ACC_MOFFSET	= SRC_ACC|DST_MOFFSET,		/* AL/AX -> moffset */
	ADDMODE_ACC_REG		= SRC_ACC|DST_REG,		/* AL/AX -> reg */
	ADDMODE_IMM		= SRC_IMM|DST_NONE,		/* immediate operand */
	ADDMODE_IMM8_RM		= SRC_IMM8|MOD_RM|DIR_REVERSED,	/* immediate -> register/memory */
	ADDMODE_IMM_ACC		= SRC_IMM|DST_ACC,		/* immediate -> AL/AX */
	ADDMODE_IMM_REG		= SRC_IMM|DST_REG,		/* immediate -> register */
	ADDMODE_IMPLIED		= SRC_NONE|DST_NONE,		/* no operands */
	ADDMODE_MOFFSET_ACC	= SRC_MOFFSET|DST_ACC,		/* moffset -> AL/AX */
	ADDMODE_REG		= SRC_REG|DST_NONE,		/* register */
	ADDMODE_SEG_REG		= SRC_SEG_REG|DST_NONE,		/* segment register */
	ADDMODE_REG_RM		= SRC_REG|MOD_RM|DIR_REVERSED,	/* register -> register/memory */
	ADDMODE_REL		= SRC_REL|DST_NONE,		/* relative */
	ADDMODE_RM_REG		= DST_REG|MOD_RM,		/* register/memory -> register */
};

struct x86_instr {
	unsigned long		nr_bytes;

	uint8_t			opcode;		/* Opcode byte */
	uint8_t			width;
	uint8_t			mod;		/* Mod */
	uint8_t			rm;		/* R/M */
	uint8_t			reg_opc;	/* Reg/Opcode */
	uint32_t		disp;		/* Address displacement */
	union {
		uint32_t		imm_data;	/* Immediate data */
		int32_t			rel_data;	/* Relative address data */
	};

	unsigned long		type;		/* See enum x86_instr_types */
	unsigned long		flags;		/* See enum x86_instr_flags */
	enum x86_seg_override	seg_override;
	enum x86_rep_prefix	rep_prefix;
	unsigned char		lock_prefix;
	struct x86_operand	src;
	struct x86_operand	dst;
};

int
arch_8086_decode_instr(struct x86_instr *instr, uint8_t* RAM, addr_t pc);

int
arch_8086_instr_length(struct x86_instr *instr);

#endif /* X86_DECODE_H */
