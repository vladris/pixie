#pragma once

#include <functional>
#include <iostream>
#include <variant>
#include <vector>

namespace pixie {

using word_t = uint16_t;

// VM op codes
enum class op_code_t : uint8_t
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
enum class register_t : uint8_t
{
	R0, R1, R2, R3, // General purpose registers
	SB, SP, // Stack
	PC // Program counter
};

const size_t c_register_count = 7;

// An address is a word
using address_t = word_t;

// A value holds a word
struct value_t
{
	word_t value;
};

// An operand can be either a register, a value, or a dereference (address)
using operand_t = std::variant<register_t, value_t, address_t>;

// VM instructions consist of an op code an up to two operands
struct instruction_t
{
	op_code_t op_code;
	operand_t m1;
	operand_t m2;
};

using input_t = std::function<word_t()>;
using output_t = std::function<void(word_t)>;

class vm_t
{
public:
	vm_t() : memory(c_memory_size) { }

	void connect_input(input_t&& input) { inputs.emplace_back(input); }
	void connect_output(output_t&& output) { outputs.emplace_back(output); }

	template <typename It>
	void run(It f, It l)
	{
		std::copy(f, l, memory.begin());

		at(register_t::SB) = at(register_t::SP) = static_cast<word_t>(std::distance(f, l));
		at(register_t::R0) = at(register_t::R1) = at(register_t::R2) = at(register_t::R3) = 0;

		// Execute until PC is at 0xFFFF
		for (at(register_t::PC) = 0; at(register_t::PC) != c_max_address; ++at(register_t::PC))
			execute_instruction(decode_instruction(at(at(register_t::PC))));
	}

private:
	const size_t c_memory_size = 0x10000;
	const address_t c_max_address = 0xFFFF;
	word_t registers[c_register_count];
	std::vector<word_t> memory;
	std::vector<input_t> inputs;
	std::vector<output_t> outputs;

	word_t& at(register_t reg) { return registers[static_cast<size_t>(reg)]; }
	word_t& at(address_t addr)	{ return memory[addr]; }

	// Given a value type, return a reference to the contained value (used by visitor below)
	word_t& at(value_t& value) { return value.value; }

	// Given an operand, forward to one of the at functions
	word_t& at(operand_t& m)
	{
		return std::visit([this](auto&& arg) -> word_t& { return at(std::forward<decltype(arg)>(arg)); }, m);
	}

	operand_t decode_operand(uint8_t operand)
	{
		// 0b1000 bit marks the operand as a dereference (address)
		if (operand & 0b1000)
			return at(decode_operand(operand & 0b0111));

		// 0b0111 marks the operand as a word value to be read from the program code
		if (operand == 0b0111)
			return value_t{ at(++at(register_t::PC)) };

		// All other possible values encode registers
		return static_cast<register_t>(operand);
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
		case op_code_t::In:  at(instr.m1) = inputs[at(instr.m2)](); break;
		case op_code_t::Out: outputs[at(instr.m1)](at(instr.m2)); break;
		}
	}
};

} // namespace pixie
