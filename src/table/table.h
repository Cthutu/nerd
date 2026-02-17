//------------------------------------------------------------------------------
// Text table rendering module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core unicode
//> def: _POSIX_C_SOURCE=200809L
//> def: _GNU_SOURCE

#pragma once

#include <core/core.h>

//------------------------------------------------------------------------------

typedef enum {
    TABLE_CELL_EMPTY,
    TABLE_CELL_TEXT,
    TABLE_CELL_U32,
    TABLE_CELL_U64,
    TABLE_CELL_I32,
    TABLE_CELL_TIME,
} TableCellKind;

typedef struct {
    TableCellKind kind;
    string        text;
    u32           u32_value;
    u64           u64_value;
    i32           i32_value;
    TimeDuration  time_value;
    usize         digits;
} TableCell;

typedef struct {
    cstr title;
    cstr colour;
} TableColumn;

typedef struct {
    Array(TableColumn) columns;
    Array(usize) widths;
    Array(TableCell) rows;
    Array(cstr) row_colours;
    Array(bool) row_divider_before;
    usize row_count;
} Table;

typedef struct {
    const cstr* colours;
    bool        divider_before;
} TableAddRowParams;

usize digits_u32(u32 n);
usize digits_u64(u64 n);
usize digits_i32(i32 n);

TableCell table_cell_empty(void);
TableCell table_cell_string(string text);
TableCell table_cell_u32(u32 value);
TableCell table_cell_u64(u64 value);
TableCell table_cell_i32(i32 value);
TableCell table_cell_time(TimeDuration value);

void table_init(Table* table, Array(TableColumn) columns);
void table_done(Table* table);

void table_reserve_rows(Table* table, usize row_count);
void _table_add_row(Table*            table,
                    const TableCell*  cells,
                    TableAddRowParams params);

#define table_add_row(table, cells, ...)                                       \
    _table_add_row((table), (cells), (TableAddRowParams){__VA_ARGS__})

void table_print(const Table* table,
                 cstr         border_colour,
                 cstr         header_colour,
                 cstr         reset);

void dump_info(void);

//------------------------------------------------------------------------------
