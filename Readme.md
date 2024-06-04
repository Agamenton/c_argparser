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
    Argparser ap = Argparser_new();
    ap.set_program_title("LZ77 compression/decompression program");
    ap.set_program_description("This is a simple program to compress and decompress files using the LZ77 algorithm");
    ap.add_opt_argument("c", "compress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to compress the input file");
    ap.add_opt_argument("d", "decompress", ARG_BOOL, (arg_value) { .b = FALSE }, "Choose this to de-compress a .lz77 file");
    ap.add_opt_argument("o", "output", ARG_STRING, (arg_value) { .s = "output.lz77" }, "Relative or full path to the output file");
    ap.add_opt_argument("w", "window", ARG_INT, (arg_value) { .i = 255 }, "Size of the sliding window");
    ap.add_pos_argument("input", ARG_STRING, "Relative or full path to the input file you want to compress/decompress");
    ap.set_rule("c", ARG_EXCLUSIVE, "d");
    ap.parse_args(argc, argv);
    
    if (ap.arg("c").b){
        compress_file(ap.arg("input").s, ap.arg("output").s, ap.arg("window").i);
    }
    else if (ap.arg("d").b){
        decompress_file(ap.arg("input").s, ap.arg("output").s);
    }
    else {
        printf("No action specified\n");
        ap.print_help();
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
Usage: a.o [-h] [-c] [-d] [-o VALUE] [-w VALUE] input
Arguments:
  input
        (string)
        Relative or full path to the input file you want to compress/decompress

  -h, --help
        (flag)
        Show this help message

  -c, --compress
        (flag)
        Choose this to compress the input file

  -d, --decompress
        (flag)
        Choose this to de-compress a .lz77 file

  -o, --output VALUE
        (string)
        Relative or full path to the output file

  -w, --window VALUE
        (int)
        Size of the sliding window

Rules:
        'c' EXCLUSIVE 'd'
```