main :: fn () {
compress_error :: fn (error: CompressionError) -> [..]u8\CompressionError {
}

result:=Err(Error.InvalidData{
message
})

counts : [16]u16
next_codes : [16]u16

for i:usize=0;
i<lengths.count;
i+=1 {
    length:=lengths[i]
}

state:=on pushed {
Err(OutputLimitExceeded)=>return compress_bool_error(OutputLimitExceeded) else=>{}
}

deflate_decode_compressed_block :: fn (reader : ^DeflateReader,
output : ^[..]u8,
literals : ^Huffman,
distances : ^Huffman,
max_output_bytes : usize) -> bool\CompressionError {
}
}
¬
main :: fn () {
    compress_error :: fn (error: CompressionError) -> [..]u8\CompressionError {
    }

    result := Err(Error.InvalidData {
        message
    })

    counts     : [16]u16
    next_codes : [16]u16

    for i: usize = 0;
        i < lengths.count;
        i += 1 {
        length := lengths[i]
    }

    state := on pushed {
        Err(OutputLimitExceeded) => return compress_bool_error(OutputLimitExceeded)
        else => {
        }
    }

    deflate_decode_compressed_block :: fn (reader           : ^DeflateReader,
                                           output           : ^[..]u8,
                                           literals         : ^Huffman,
                                           distances        : ^Huffman,
                                           max_output_bytes : usize) -> bool\CompressionError {
    }
}
