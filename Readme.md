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
    ap_set_program_title("LZ77 compression/decompression program");
    ap_set_program_description("This is a simple program to compress and decompress files using the LZ77 algorithm");
    ap_add_opt_argument("c", "compress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to compress the input file");
    ap_add_opt_argument("d", "decompress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to de-compress a .lz77 file");
    ap_add_opt_argument("o", "output", ARG_STRING, (arg_value) { .s = "output.lz77" }, "Relative or full path to the output file");
    ap_add_opt_argument("w", "window", ARG_INT, (arg_value) { .i = 255 }, "Size of the sliding window");
    ap_add_pos_argument("input", "Relative or full path to the input file you want to compress/decompress");
    ap_set_rule("c", ARG_EXCLUSIVE, "d");
    ap_parse_args(argc, argv);
    
    if (arg("c").b){
        compress_file(arg("input").s, arg("output").s, arg("window").i);
    }
    else if (arg("d").b){
        decompress_file(arg("input").s, arg("output").s);
    }
    else {
        printf("No action specified\n");
        ap_print_help();
    }

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
LZ77 compression/decompression program
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