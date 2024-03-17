# argparser

## Description
This is a simple command line argument parser for C. 

It is designed to be easy to use and to provide a simple way to parse command line arguments.

Inspired by the Python argparse module.

## Usage example
Code:
```c
#include "argparser.h"

int main(int argc, char *argv[]) {
    // argparser setup 
    set_program_name("lz77.o");
    set_program_description("This is a simple program to compress and decompress files using the LZ77 algorithm");
    add_optional_argument("c", "compress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to compress the input file");
    add_optional_argument("d", "decompress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to de-compress a .lz77 file");
    add_optional_argument("o", "output", ARG_STRING, (arg_value) { .s = "output.lz77" }, "Relative or full path to the output file");
    add_optional_argument("w", "window", ARG_INT, (arg_value) { .i = 255 }, "Size of the sliding window");
    add_positional_argument("input", "Relative or full path to the input file you want to compress/decompress");
    parse_args(argc, argv);

    return 0;
}
```

commandline:
```c
// -h or --help is automatically included argument that prints the help message
lz77.o -h
```

output:
```
lz77.o
This is a simple program to compress and decompress files using the LZ77 algorithm
Usage: lz77.o [-h] [-c] [-d] [-o VALUE] [-w VALUE] input
Arguments:
  input
        Relative or full path to the input file you want to compress/decompress

  -h, --help
        Show this help message

  -c, --compress
        Choose this to compress the input file

  -d, --decompress
        Choose this to de-compress a .lz77 file

  -o, --output VALUE
        Relative or full path to the output file

  -w, --window VALUE
        Size of the sliding window
```