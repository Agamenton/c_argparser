#include "argparser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
    char* short_name;
    char* long_name;
    arg_type type;
    arg_value value;
    char* help;
    char optional;
} opt_c_argument;


typedef struct {
    char* name;
    char* help;
    arg_value value;
} pos_c_argument;


char* program_dsc = NULL;
char* program_name = NULL;


opt_c_argument optional_arguments[127] = {
        { .short_name = "-h", .long_name = "--help", .type = ARG_BOOL, .value = (arg_value) { .b = FALSE }, .help = "Show this help message" },
};
pos_c_argument positional_arguments[127];
char opt_arguments_count = 1;
char pos_arguments_count = 0;


// Private function to find index of an optional argument by name
static int find_opt_argument(char* name);

// Private function to find index of a positional argument by name
static int find_pos_argument(char* name);

// Private function to check if an argument exists
static int exists(char* s_name, char* l_name);

// Skips the first dash(es) and returns only the name
static char* extract_name(char* arg);


static int exists(char* s_name, char* l_name)
{
    if (find_opt_argument(s_name) != -1 ||
        find_opt_argument(l_name) != -1 ||
        find_pos_argument(s_name) ||
        find_pos_argument(l_name))
    {
        return TRUE;
    }
    return FALSE;
}


static int find_opt_argument(char* name)
{
    for (int i = 0; i < opt_arguments_count; i++)
    {
        if (strcmp(optional_arguments[i].short_name, name) == 0 || strcmp(optional_arguments[i].long_name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}


static int find_pos_argument(char* name)
{
    for (int i = 0; i < pos_arguments_count; i++)
    {
        if (strcmp(positional_arguments[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}


static char* extract_name(char* arg)
{
    int i = 0;
    while (arg[i] == '-')
    {
        i++;
    }
    return arg + i;
}


int add_optional_argument(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help)
{
    if (opt_arguments_count >= 127 || exists(short_name, long_name) == TRUE)    // explicit comparison with TRUE in case TRUE is defined as 0
    {
        return -1;
    }

    opt_c_argument argument;
    argument.short_name = short_name;
    argument.long_name = long_name;
    argument.type = type;
    argument.value = default_value;
    argument.help = help;
    argument.optional = TRUE;
    optional_arguments[opt_arguments_count] = argument;
    opt_arguments_count++;

    return opt_arguments_count;
}


int add_positional_argument(char* name, char* help)
{
    if (pos_arguments_count >= 127 || exists(name, name) == TRUE)
    {
        return -1;
    }

    pos_c_argument argument;
    argument.name = name;
    argument.help = help;
    argument.value = (arg_value) { .s = NULL };
    positional_arguments[pos_arguments_count] = argument;
    pos_arguments_count++;

    return pos_arguments_count;
}


arg_value arg(char* name)
{
    int index = find_opt_argument(name);
    if (index == -1)
    {
        index = find_pos_argument(name);
        if (index == -1)
        {
            fprintf(stderr, "Unknown argument: %s\n", name);
            exit(ARG_ERROR_CODE);
        }
        return positional_arguments[index].value;
    }
    return optional_arguments[index].value;
}


void parse_args(int argc, char* argv[])
{
    int current_pos_arg = 0;
    int arg_idx = 1;
    while(arg_idx < argc)
    {
        char* current_word = argv[arg_idx];
        if (current_word[0] == '-')  // Optional arguments
        {
            char* name = extract_name(current_word);
            int index = find_opt_argument(name);
            if (index == -1)
            {
                fprintf(stderr, "Unknown argument: %s\n", name);
                exit(ARG_ERROR_CODE);
            }
            if (optional_arguments[index].type == ARG_BOOL)
            {
                optional_arguments[index].value.b = !optional_arguments[index].value.b;
            }
            else
            {
                arg_idx++;
                if (arg_idx >= argc)
                {
                    fprintf(stderr, "Expected value for argument: %s\n", name);
                    exit(ARG_ERROR_CODE);
                }
                char* value = argv[arg_idx];
                if (optional_arguments[index].type == ARG_INT)
                {
                    optional_arguments[index].value.i = atoi(value);
                }
                else if (optional_arguments[index].type == ARG_FLOAT)
                {
                    optional_arguments[index].value.f = atof(value);
                }
                else if (optional_arguments[index].type == ARG_STRING)
                {
                    optional_arguments[index].value.s = value;
                }
            }
        }
        else
        {
            if (current_pos_arg >= pos_arguments_count)
            {
                fprintf(stderr, "Too many positional arguments\n");
                exit(ARG_ERROR_CODE);
            }
            positional_arguments[current_pos_arg].value.s = current_word;
            current_pos_arg++;
        }
        arg_idx++;
    }
    if (current_pos_arg < pos_arguments_count)
    {
        fprintf(stderr, "Not enough positional arguments\n");
        exit(ARG_ERROR_CODE);
    }

    if (arg("-h").b == TRUE)
    {
        print_help();
        exit(0);
    }
}


void set_program_description(char* dsc)
{
    program_dsc = dsc;
}


void set_program_name(char* name)
{
    program_name = name;
}


void print_help()
{
    char* local_program_name = program_name == NULL ? "program_name" : program_name;
    char* local_program_dsc = program_dsc == NULL ? "<No description>" : program_dsc;

    printf("%s\n", local_program_name);
    printf("%s\n", local_program_dsc);
    printf("Usage: %s ", local_program_name);
    for (int i = 0; i < pos_arguments_count; i++)
    {
        printf("%s ", positional_arguments[i].name);
    }
    for (int i = 0; i < opt_arguments_count; i++)
    {
        if(optional_arguments[i].type == ARG_BOOL)
        {
            printf("[%s] ", optional_arguments[i].short_name);
        }
        else
        {
            printf("[%s VALUE] ", optional_arguments[i].short_name);
        }
    }
    printf("\n");
}