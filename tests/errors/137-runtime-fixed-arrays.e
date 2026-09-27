main :: fn () { n := 2.5
values: [n]i32 }
¬
{
    "message": "Type mismatch: expected `integer array length`, found `f64`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "This expression has type `f64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
n: i32 = 3
values: [n]i32
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
            "message": "This expression has type `runtime array length`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
work :: fn (n: i32, values: [n]i32) {}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "This expression has type `runtime array length`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () { n := 2
values: [n]i32 = [1, 2] }
¬
{
    "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
            "message": "This expression has type `runtime array length`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () { n := 2
values: [n]i32
values = nil }
¬
{
    "message": "Cannot assign to `runtime-sized fixed array; assign elements instead`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 1,
            "length": 6,
            "message": "`runtime-sized fixed array; assign elements instead` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `runtime-sized fixed array; assign elements instead` as a variable with `:` or assign to a different mutable symbol."
    ]
}
¬
main :: fn () { n := 2
values: [n]i32
values.resize_to(3) }
¬
{
    "message": "Type mismatch: expected `slice field .data, .count, .bytes, or defined method`, found `resize_to`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 8,
            "length": 9,
            "message": "This expression has type `resize_to`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () { n := 2
values: [n]i32
values.free() }
¬
{
    "message": "Type mismatch: expected `slice field .data, .count, .bytes, or defined method`, found `free`",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 3,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 8,
            "length": 4,
            "message": "This expression has type `free`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
work :: fn (n: i32) -> []i32 { values: [n]i32
return values }
main :: fn () {}
¬
{
    "message": "LLVM tool reported an error while compiling generated IR (exit code 1)\nMessage: '%t0' defined with type 'i32' but expected 'i64'\nLocation: llc: error: llc: /home/matt/nerd/tests/errors/137-runtime-fixed-arrays.7.input.link.ll:19:45\nGenerated LLVM: /home/matt/nerd/tests/errors/137-runtime-fixed-arrays.7.input.link.ll\nRuntime object: (none)\nCommand: llc --mtriple=x86_64-unknown-linux-gnu -filetype=obj -relocation-model=pic -O0 -o \"/home/matt/nerd/tests/errors/137-runtime-fixed-arrays.7.input.obj.o\" \"/home/matt/nerd/tests/errors/137-runtime-fixed-arrays.7.input.link.ll\"\nSource:\n  %t1 = call ptr @nrt_local_array_alloc(i64 %t0, i64 4, ptr @.macro.file.m0, i32 1), !dbg !10\n                                            ^",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [],
    "notes": [],
    "help": []
}
¬
main :: fn () { n: i32 = undefined
values: [n]i32 }
¬
{
    "message": "Cannot read `n` before it has been assigned",
    "source_file": "tests/errors/137-runtime-fixed-arrays.e",
    "primary_location": {
        "line": 2,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 10,
            "length": 1,
            "message": "`n` is read here"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 17,
            "length": 1,
            "message": "`n` is declared with `undefined` here"
        }
    ],
    "notes": [
        "Variables declared with `undefined` must be assigned before they are read."
    ],
    "help": [
        "Assign to `n` on every path before using its value."
    ]
}
