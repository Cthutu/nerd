/*
------------------------------------------------------------------------------
Nerd Compiler - IR Generation
------------------------------------------------------------------------------
Purpose
  Builds a minimal intermediate representation from lexer and AST data.

Responsibilities
  - Define IR instruction and value model types.
  - Emit assignment and return instructions for supported AST nodes.
  - Provide IR generation from front-end outputs and readable debug table output.

Primary Public Procedures
  - ir_add_assign Emit an assignment instruction into the IR stream.
  - ir_add_return Emit a return instruction into the IR stream.
  - ir_generate   Build IR instructions from lexer and AST artefacts.
  - ir_dump       Print IR output as a diagnostic table.
------------------------------------------------------------------------------
*/
package main

import "core:fmt"

Ir :: struct {
	instructions: [dynamic]IrInstruction,
}

IrOperation :: enum {
	Assign,
	Return,
}

IrValueKind :: enum {
	Variable,
	Integer,
}

IrValue :: struct {
	kind:  IrValueKind,
	value: union {
		u64,
	},
}

IrInstruction :: struct {
	op:     IrOperation,
	lvalue: IrValue,
	rvalue: [2]IrValue,
}

ir_value_text :: proc(value: IrValue) -> string {
	switch value.kind {
	case IrValueKind.Variable:
		return fmt.tprintf("%%%d", value.value.(u64))
	case IrValueKind.Integer:
		return fmt.tprintf("%d", value.value.(u64))
	case:
		return "<unknown>"
	}
}

ir_instruction_text :: proc(instr: IrInstruction) -> string {
	switch instr.op {
	case IrOperation.Assign:
		return fmt.tprintf("%s = %s", ir_value_text(instr.lvalue), ir_value_text(instr.rvalue[0]))
	case IrOperation.Return:
		return fmt.tprintf("ret %s", ir_value_text(instr.rvalue[0]))
	case:
		return "<unknown>"
	}
}

ir_dump :: proc(ir: Ir) {
	border_colour := "\x1b[38;5;245m"
	header_colour := "\x1b[1;38;5;45m"
	index_colour := "\x1b[38;5;214m"
	instruction_colour := "\x1b[38;5;111m"
	reset := "\x1b[0m"

	columns := [2]TableColumn {
		{title = "index", colour = index_colour},
		{title = "instruction", colour = instruction_colour},
	}

	table := table_init(columns[:])
	table_reserve_rows(&table, len(ir.instructions))

	for i in 0 ..< len(ir.instructions) {
		instruction := ir.instructions[i]
		row := [2]TableCell {
			table_cell_u32(u32(i)),
			table_cell_text(ir_instruction_text(instruction)),
		}
		table_add_row(&table, row[:])
	}

	table_print(table, border_colour, header_colour, reset)
}

ir_add_assign :: proc(ir: ^Ir, lvalue_index: u64, rvalue: IrValue) {
	append(
		&ir.instructions,
		IrInstruction {
			IrOperation.Assign,
			IrValue{IrValueKind.Variable, lvalue_index},
			[2]IrValue{rvalue, IrValue{}},
		},
	)
}

ir_add_return :: proc(ir: ^Ir, rvalue: IrValue) {
	append(
		&ir.instructions,
		IrInstruction{IrOperation.Return, IrValue{}, [2]IrValue{rvalue, IrValue{}}},
	)
}

ir_generate :: proc(lex: Lexer, ast: Ast) -> Ir {
	ir: Ir
	index: u64 = 0

	for node in ast.nodes {
		switch node.kind {
		case AstKind.IntLit:
			ir_add_assign(&ir, index, IrValue{IrValueKind.Integer, ast_get_int(lex, node)})
			index += 1
		}
	}

	ir_add_return(&ir, IrValue{IrValueKind.Variable, index - 1})
	return ir
}
