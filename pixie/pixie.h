#pragma once

#include <iostream>
#include <variant>
#include <vector>

namespace pixie {

using byte_t = uint8_t;
using word_t = uint16_t;

// VM op codes
enum class op_code_t : byte_t
{
	Mov, // Move data
	Add, Sub, Mul, Div, Rem, // Arithmetic
	Not, // Logical
	And, Or, Xor, // Bitwise
	Eq, Le, Leq, // Comparisons
	Jnz, // Flow control
	In, Out // IO
};

const size_t c_op_count = 16;

// VM registers
enum class register_t : byte_t
{
	R0, R1, R2, R3, // General purpose registers
	SB, SP, // Stack
	PC // Program counter
};

const size_t c_register_count = 7;

// Dereference holds a reference to a memory location
struct dereference_t
{
	word_t addr;
};

// An operand can be either a register, a word, or a dereference
using operand_t = std::variant<register_t, word_t, dereference_t>;

// VM instructions consist of an op code an up to two operands
struct instruction_t
{
	op_code_t op_code;
	operand_t m1;
	operand_t m2;
};

class vm_t
{
public:
	vm_t() : memory(c_memory_size) { }

	template <typename It>
	void load(It f, It l)
	{
		std::copy(f, l, memory.begin());

		at(register_t::PC) = 0;
		at(register_t::SB) = at(register_t::SP) = std::distance(f, l);
		at(register_t::R0) = at(register_t::R1) = at(register_t::R2) = at(register_t::R3) = 0;
	}

	void run()
	{
		// Execute until PC is at 0xFFFF
		for (; at(register_t::PC) != max_word; ++at(register_t::PC))
			execute_instruction(decode_instruction(at(at(register_t::PC))));
	}

private:
	const size_t c_memory_size = 0x10000;
	const word_t max_word = 0xFFFF;
	word_t registers[c_register_count];
	std::vector<word_t> memory;

	word_t& at(register_t reg) { return registers[static_cast<size_t>(reg)]; }
	word_t& at(word_t addr)	{ return memory[addr]; }

	struct operand_visitor_t
	{
		word_t& operator()(register_t reg) { return vm.at(reg); }
		word_t& operator()(word_t& addr) { return addr; }
		word_t& operator()(dereference_t& deref) { return vm.at(deref.addr); }

		vm_t& vm;
	};
	word_t& at(operand_t& m) { return std::visit(operand_visitor_t{ *this }, m); }

	word_t in() { word_t input; std::cin >> input; return input; }
	void out(word_t output) { std::cout << output; }

	operand_t decode_operand(byte_t encoded_operand)
	{
		// 0b1000 bit marks the operand as a dereference
		bool deref = encoded_operand & 8;
		encoded_operand &= 7;
		operand_t result;

		// 0b0111 marks the operand as a word to be read from the program code
		if (encoded_operand == 7)
			result = at(++at(register_t::PC));
		// All other possible values encode registers
		else
			result = static_cast<register_t>(encoded_operand);
	
		// Once operand is initialized, dereference it if it is a dereference
		if (deref)
			result = dereference_t{ at(result) };

		return result;
	}

	instruction_t decode_instruction(word_t word)
	{
		// 0xFF00 bits encode the op code, 0x00F0 and 0x000F the operands
		return {
			static_cast<op_code_t>((word >> 8) & 0xFF),
			decode_operand((word >> 4) & 0xF),
			decode_operand(word & 0xF)
		};
	}

	void execute_instruction(instruction_t& instr)
	{
		switch (instr.op_code)
		{
		case op_code_t::Mov: at(instr.m1) = at(instr.m2); break;
		case op_code_t::Add: at(instr.m1) += at(instr.m2); break;
		case op_code_t::Sub: at(instr.m1) -= at(instr.m2); break;
		case op_code_t::Mul: at(instr.m1) *= at(instr.m2); break;
		case op_code_t::Div: at(instr.m1) /= at(instr.m2); break;
		case op_code_t::Rem: at(instr.m1) %= at(instr.m2); break;
		case op_code_t::Not: at(instr.m1) = !at(instr.m2); break;
		case op_code_t::And: at(instr.m1) &= at(instr.m2); break;
		case op_code_t::Or:  at(instr.m1) |= at(instr.m2); break;
		case op_code_t::Xor: at(instr.m1) ^= at(instr.m2); break;
		case op_code_t::Eq:  at(instr.m1) = at(instr.m1) == at(instr.m2); break;
		case op_code_t::Le:  at(instr.m1) = at(instr.m1) < at(instr.m2); break;
		case op_code_t::Leq: at(instr.m1) = at(instr.m1) <= at(instr.m2); break;
		case op_code_t::Jnz: if (at(instr.m1)) at(register_t::PC) = at(instr.m2) - 1; break;
		case op_code_t::In:  at(instr.m1) = in(); break;
		case op_code_t::Out: out(at(instr.m1)); break;
		}
	}
};

} // namespace pixie
