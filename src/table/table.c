//------------------------------------------------------------------------------
// Table rendering implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <table/table.h>
#include <unicode/unicode.h>

#include <stdio.h>

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

    bool  sign = n < 0;
    u32   mag  = sign ? (u32)(-(i64)n) : (u32)n;
    usize d    = 0;

    while (mag > 0) {
        mag /= 10;
        d++;
    }

    return sign ? d + 1 : d;
}

internal usize format_time_duration(char*        buffer,
                                    usize        buffer_size,
                                    TimeDuration value)
{
    f64  amount = 0.0;
    cstr unit   = "ns";

    if (value >= time_from_secs(1)) {
        amount = time_secs(value);
        unit   = "s";
    } else if (value >= time_from_ms(1)) {
        amount = time_msecs(value);
        unit   = "ms";
    } else if (value >= time_from_us(1)) {
        amount = time_usecs(value);
        unit   = "μs";
    } else {
        amount = time_nsecs(value);
        unit   = "ns";
    }

    int written = snprintf(buffer, buffer_size, "%.3f %s", amount, unit);
    if (written < 0) {
        return 0;
    }
    usize max_len = buffer_size > 0 ? buffer_size - 1 : 0;
    usize len     = (usize)written;
    return len <= max_len ? len : max_len;
}

internal usize time_duration_text_width(TimeDuration value)
{
    char buf[64];
    return format_time_duration(buf, sizeof(buf), value);
}

TableCell table_cell_empty(void)
{
    return (TableCell){
        .kind = TABLE_CELL_EMPTY,
    };
}

TableCell table_cell_string(string text)
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

TableCell table_cell_time(TimeDuration value)
{
    return (TableCell){
        .kind       = TABLE_CELL_TIME,
        .time_value = value,
        .digits     = time_duration_text_width(value),
    };
}

void _table_init(Table* table,
                 Array(TableColumn) columns,
                 TableInitParams params)
{
    *table                 = (Table){0};

    table->title           = params.title ? params.title : "";
    table->title_fg_colour = params.title_foreground_colour
                                 ? params.title_foreground_colour
                                 : ANSI_BOLD_WHITE;
    table->title_bg_colour = params.title_background_colour
                                 ? params.title_background_colour
                                 : ANSI_BG_BLUE;

    usize column_count     = array_count(columns);
    if (column_count == 0) {
        return;
    }

    array_requires_capacity(table->columns, column_count);
    array_requires_capacity(table->widths, column_count);

    for (usize i = 0; i < column_count; i++) {
        cstr title = columns[i].title ? columns[i].title : "";
        array_push(table->columns, columns[i]);
        array_push(table->widths,
                   unicode_utf8_string_cell_width(string_from_cstr(title)));
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

    array_requires_capacity(table->rows, row_count * col_count);
    array_requires_capacity(table->row_colours, row_count * col_count);
    array_requires_capacity(table->row_divider_before, row_count);
}

void _table_add_row(Table*            table,
                    const TableCell*  cells,
                    TableAddRowParams params)
{
    usize col_count = array_count(table->columns);
    if (col_count == 0 || !cells) {
        return;
    }

    for (usize i = 0; i < col_count; i++) {
        const TableCell* cell  = &cells[i];
        usize            width = 1;

        switch (cell->kind) {
        case TABLE_CELL_EMPTY:
            width = 1;
            break;
        case TABLE_CELL_TEXT:
            width = unicode_utf8_string_cell_width(cell->text);
            break;
        case TABLE_CELL_U32:
        case TABLE_CELL_U64:
        case TABLE_CELL_I32:
        case TABLE_CELL_TIME:
            width = cell->digits;
            break;
        default:
            width = 1;
            break;
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

internal usize table_content_width(const usize* widths, usize width_count)
{
    if (width_count == 0) {
        return 0;
    }

    usize total = 0;
    for (usize i = 0; i < width_count; i++) {
        total += widths[i] + 2;
    }
    total += width_count - 1;
    return total;
}

internal void print_span_line(
    cstr left, cstr right, usize width, cstr border_colour, cstr reset)
{
    pr("%s%s", border_colour, left);
    print_repeat("─", width);
    pr("%s%s\n", right, reset);
}

internal void print_title_row(cstr  title,
                              usize width,
                              cstr  fg_colour,
                              cstr  bg_colour,
                              cstr  border_colour,
                              cstr  reset)
{
    string title_text  = string_from_cstr(title ? title : "");
    usize  title_width = unicode_utf8_string_cell_width(title_text);

    pr("%s│%s", border_colour, reset);
    pr("%s%s ", bg_colour ? bg_colour : "", fg_colour ? fg_colour : "");
    pr("%.*s", STRINGV(title_text));
    if (width > (title_width + 1)) {
        print_repeat(" ", width - title_width - 1);
    }
    pr("%s", reset);
    pr("%s│%s\n", border_colour, reset);
}

internal void print_text_cell(string text, usize width, cstr colour, cstr reset)
{
    pr(" %s%.*s%s", colour, STRINGV(text), reset);
    usize text_width = unicode_utf8_string_cell_width(text);
    if (width > text_width) {
        print_repeat(" ", width - text_width);
    }
    pr(" ");
}

internal void
print_u32_cell(u32 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%u%s", colour, value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void
print_u64_cell(u64 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%llu%s", colour, (unsigned long long)value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void
print_i32_cell(i32 value, usize width, usize digits, cstr colour, cstr reset)
{
    pr(" %s%d%s", colour, value, reset);
    print_repeat(" ", width - digits);
    pr(" ");
}

internal void
print_time_cell(TimeDuration value, usize width, cstr colour, cstr reset)
{
    char  buf[64];
    usize len = format_time_duration(buf, sizeof(buf), value);
    print_text_cell(string_from((u8*)buf, len), width, colour, reset);
}

void _table_print(const Table* table, TablePrintParams params)
{
    cstr border_colour =
        params.border_colour ? params.border_colour : ANSI_FAINT_WHITE;
    cstr header_colour =
        params.header_colour ? params.header_colour : ANSI_BOLD_WHITE;
    cstr reset      = ANSI_RESET;

    usize col_count = array_count(table->columns);
    if (col_count == 0) {
        return;
    }

    Array(usize) widths = NULL;
    array_requires_capacity(widths, col_count);
    for (usize i = 0; i < col_count; i++) {
        array_push(widths, table->widths[i]);
    }

    usize content_width = table_content_width(widths, col_count);
    bool  has_title     = table->title && table->title[0] != '\0';
    if (has_title) {
        usize title_width =
            unicode_utf8_string_cell_width(string_from_cstr(table->title));
        usize min_width = title_width + 2;
        if (content_width < min_width) {
            widths[col_count - 1] += (min_width - content_width);
            content_width = min_width;
        }

        print_span_line("┌", "┐", content_width, border_colour, reset);
        print_title_row(table->title,
                        content_width,
                        table->title_fg_colour,
                        table->title_bg_colour,
                        border_colour,
                        reset);
        print_line("├", "┬", "┤", widths, col_count, border_colour, reset);
    } else {
        print_line("┌", "┬", "┐", widths, col_count, border_colour, reset);
    }

    pr("%s", border_colour);
    for (usize i = 0; i < col_count; i++) {
        cstr title = table->columns[i].title ? table->columns[i].title : "";
        pr("│%s", reset);
        print_text_cell(
            string_from_cstr(title), widths[i], header_colour, reset);
        pr("%s", border_colour);
    }
    pr("│\n");

    print_line("├", "┼", "┤", widths, col_count, border_colour, reset);

    for (usize r = 0; r < table->row_count; r++) {
        if (array_count(table->row_divider_before) == table->row_count &&
            table->row_divider_before[r]) {
            print_line("├", "┼", "┤", widths, col_count, border_colour, reset);
        }

        usize row_offset = r * col_count;

        pr("%s", border_colour);
        for (usize c = 0; c < col_count; c++) {
            const TableCell* cell   = &table->rows[row_offset + c];
            usize            width  = widths[c];
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
                print_u32_cell(
                    cell->u32_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_U64:
                print_u64_cell(
                    cell->u64_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_I32:
                print_i32_cell(
                    cell->i32_value, width, cell->digits, colour, reset);
                break;
            case TABLE_CELL_TIME:
                print_time_cell(cell->time_value, width, colour, reset);
                break;
            default:
                print_text_cell(string_from_cstr("?"), width, "", reset);
                break;
            }

            pr("%s", border_colour);
        }
        pr("│\n");
    }

    print_line("└", "┴", "┘", widths, col_count, border_colour, reset);
    array_free(widths);
}

//------------------------------------------------------------------------------
