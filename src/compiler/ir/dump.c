//------------------------------------------------------------------------------
// IR dumping
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <compiler/ir/ir.h>
#include <table/table.h>

//------------------------------------------------------------------------------

internal string ir_value_text(const IrValue* value)
{
    StringBuilder sb = {0};
    sb_init(&sb, &temp_arena);
    switch (value->kind) {
    case IR_VALUE_VARIABLE:
        sb_format(&sb, "$%u", (u32)value->value.integer);
        break;
    case IR_VALUE_INTEGER:
        sb_format(&sb, "%u", (u32)value->value.integer);
        break;
    default:
        sb_format(&sb, "");
        break;
    }
    return sb_to_string(&sb);
}

void ir_dump(const Ir* ir)
{
    Array(TableColumn) columns = NULL;
    array_push(columns,
               (TableColumn){.title = "Index", .colour = ANSI_CYAN},
               (TableColumn){.title = "Op", .colour = ANSI_GREEN},
               (TableColumn){.title = "LValue", .colour = ANSI_YELLOW},
               (TableColumn){.title = "RValue[0]", .colour = ANSI_MAGENTA},
               (TableColumn){.title = "RValue[1]", .colour = ANSI_BLUE});

    Table table = {0};
    table_init(&table, columns, .title = "IR Instructions (56 bytes each)");
    table_reserve_rows(&table, array_count(ir->instructions));
    array_free(columns);

    for (usize i = 0; i < array_count(ir->instructions); ++i) {
        TableCell      row[5];
        IrInstruction* instr = &ir->instructions[i];

        // Index
        row[0]               = table_cell_u32((u32)i);

        // Operation
        switch (instr->op) {
        case IR_OP_ASSIGN:
            row[1] = table_cell_string(s("ASSIGN"));
            break;
        case IR_OP_RETURN:
            row[1] = table_cell_string(s("RETURN"));
            break;
        default:
            row[1] = table_cell_string(s("UNKNOWN"));
            break;
        }

        // LValue
        row[2] = table_cell_string(ir_value_text(&instr->lvalue));

        // RValue[0]
        row[3] = table_cell_string(ir_value_text(&instr->rvalue[0]));

        // RValue[1]
        row[4] = table_cell_string(ir_value_text(&instr->rvalue[1]));

        table_add_row(&table, row);
    }

    table_print(&table);
    table_done(&table);
}
