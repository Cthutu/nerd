//------------------------------------------------------------------------------
// Table rendering implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <table/table.h>

//------------------------------------------------------------------------------

usize digits_u32(u32 n)
{
    if (n == 0) {
        return 1;
    }

    usize d = 0;
    while (n > 0) {
        n /= 10;
        d++;
    }
    return d;
}

usize digits_u64(u64 n)
{
    if (n == 0) {
        return 1;
    }

    usize d = 0;
    while (n > 0) {
        n /= 10;
        d++;
    }
    return d;
}

usize digits_i32(i32 n)
{
    if (n == 0) {
        return 1;
    }

    bool sign = n < 0;
    u32  mag  = sign ? (u32)(-(i64)n) : (u32)n;
    usize d   = 0;

    while (mag > 0) {
        mag /= 10;
        d++;
    }

    return sign ? d + 1 : d;
}

TableCell table_cell_empty(void)
{
    return (TableCell){
        .kind = TABLE_CELL_EMPTY,
    };
}

TableCell table_cell_text(string text)
{
    return (TableCell){
        .kind = TABLE_CELL_TEXT,
        .text = text,
    };
}

TableCell table_cell_u32(u32 value)
{
    return (TableCell){
        .kind      = TABLE_CELL_U32,
        .u32_value = value,
        .digits    = digits_u32(value),
    };
}

TableCell table_cell_u64(u64 value)
{
    return (TableCell){
        .kind      = TABLE_CELL_U64,
        .u64_value = value,
        .digits    = digits_u64(value),
    };
}

TableCell table_cell_i32(i32 value)
{
    return (TableCell){
        .kind      = TABLE_CELL_I32,
        .i32_value = value,
        .digits    = digits_i32(value),
    };
}

TableCell table_cell_token(string kind_name, u32 offset)
{
    return (TableCell){
        .kind      = TABLE_CELL_TOKEN,
        .text      = kind_name,
        .u32_value = offset,
        .digits    = digits_u32(offset),
    };
}

void table_init(Table* table, const TableColumn* columns, usize column_count)
{
    *table = (Table){0};

    if (column_count == 0) {
        return;
    }

    array_requires(table->columns, column_count);
    array_requires(table->widths, column_count);

    for (usize i = 0; i < column_count; i++) {
        array_push(table->columns, columns[i]);
        array_push(table->widths, columns[i].title.count);
    }
}

void table_done(Table* table)
{
    if (!table) {
        return;
    }

    array_free(table->columns);
    array_free(table->widths);
    array_free(table->rows);
    array_free(table->row_colours);
    array_free(table->row_divider_before);
    *table = (Table){0};
}

void table_reserve_rows(Table* table, usize row_count)
{
    usize col_count = array_count(table->columns);
    if (col_count == 0) {
        return;
    }

    array_requires(table->rows, row_count * col_count);
    array_requires(table->row_colours, row_count * col_count);
    array_requires(table->row_divider_before, row_count);
}

void _table_add_row(Table* table, const TableCell* cells, TableAddRowParams params)
{
    usize col_count = array_count(table->columns);
    if (col_count == 0 || !cells) {
        return;
    }

    for (usize i = 0; i < col_count; i++) {
        const TableCell* cell  = &cells[i];
        usize            width = 1;

        switch (cell->kind) {
        case TABLE_CELL_EMPTY: width = 1; break;
        case TABLE_CELL_TEXT: width = cell->text.count; break;
        case TABLE_CELL_U32:
        case TABLE_CELL_U64:
        case TABLE_CELL_I32: width = cell->digits; break;
        case TABLE_CELL_TOKEN: width = cell->text.count + 1 + cell->digits; break;
        default: width = 1; break;
        }

        if (width > table->widths[i]) {
            table->widths[i] = width;
        }
    }

    for (usize i = 0; i < col_count; i++) {
        array_push(table->rows, cells[i]);
    }

    if (params.colours) {
        for (usize i = 0; i < col_count; i++) {
            array_push(table->row_colours, params.colours[i]);
        }
    } else {
        for (usize i = 0; i < col_count; i++) {
            array_push(table->row_colours, "");
        }
    }

    array_push(table->row_divider_before, params.divider_before);
    table->row_count++;
}

internal void print_repeat(cstr ch, usize count)
{
    for (usize i = 0; i < count; i++) {
        pr("%s", ch);
    }
}

internal void print_line(cstr         left,
                         cstr         mid,
                         cstr         right,
                         const usize* widths,
                         usize        width_count,
                         cstr         border_colour,
                         cstr         reset)
{
    pr("%s%s", border_colour, left);
    for (usize i = 0; i < width_count; i++) {
        print_repeat("─", widths[i] + 2);
        if (i + 1 < width_count) {
            pr("%s", mid);
        }
    }
    pr("%s%s\n", right, reset);
}

internal void print_text_cell(string text, usize width, cstr colour, cstr reset)
{
    pr(" %s%.*s%s", colour, STRINGV(text), reset);
    print_repeat(" ", width - text.count);
    pr(" ");
}

internal void print_u32_cell(u32 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%u%s", colour, value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void print_u64_cell(u64 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%llu%s", colour, (unsigned long long)value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void print_i32_cell(i32 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%d%s", colour, value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void print_token_cell(string kind_name,
                               u32    offset,
                               usize  width,
                               usize  offset_digits,
                               cstr   colour,
                               cstr   reset)
{
    pr(" %s%.*s@%u%s", colour, STRINGV(kind_name), offset, reset);
    print_repeat(" ", width - (kind_name.count + 1 + offset_digits));
    pr(" ");
}

void table_print(const Table* table, cstr border_colour, cstr header_colour, cstr reset)
{
    usize col_count = array_count(table->columns);
    if (col_count == 0) {
        return;
    }

    print_line("┌", "┬", "┐", table->widths, col_count, border_colour, reset);

    pr("%s", border_colour);
    for (usize i = 0; i < col_count; i++) {
        pr("│%s", reset);
        print_text_cell(table->columns[i].title,
                        table->widths[i],
                        header_colour,
                        reset);
        pr("%s", border_colour);
    }
    pr("│\n");

    print_line("├", "┼", "┤", table->widths, col_count, border_colour, reset);

    for (usize r = 0; r < table->row_count; r++) {
        if (array_count(table->row_divider_before) == table->row_count &&
            table->row_divider_before[r]) {
            print_line(
                "├", "┼", "┤", table->widths, col_count, border_colour, reset);
        }

        usize row_offset = r * col_count;

        pr("%s", border_colour);
        for (usize c = 0; c < col_count; c++) {
            const TableCell* cell = &table->rows[row_offset + c];
            usize            width = table->widths[c];
            cstr             colour = table->columns[c].colour;

            if (array_count(table->row_colours) == array_count(table->rows)) {
                cstr row_colour = table->row_colours[row_offset + c];
                if (row_colour && row_colour[0] != '\0') {
                    colour = row_colour;
                }
            }

            pr("│%s", reset);

            switch (cell->kind) {
            case TABLE_CELL_EMPTY:
                print_text_cell(string_from_cstr("-"), width, colour, reset);
                break;
            case TABLE_CELL_TEXT:
                print_text_cell(cell->text, width, colour, reset);
                break;
            case TABLE_CELL_U32:
                print_u32_cell(cell->u32_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_U64:
                print_u64_cell(cell->u64_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_I32:
                print_i32_cell(cell->i32_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_TOKEN:
                print_token_cell(cell->text,
                                 cell->u32_value,
                                 width,
                                 cell->digits,
                                 colour,
                                 reset);
                break;
            default: print_text_cell(string_from_cstr("?"), width, "", reset); break;
            }

            pr("%s", border_colour);
        }
        pr("│\n");
    }

    print_line("└", "┴", "┘", table->widths, col_count, border_colour, reset);
}

//------------------------------------------------------------------------------
