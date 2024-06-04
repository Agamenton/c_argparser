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
    char used;
} opt_c_argument;


typedef struct {
    char* name;
    char* help;
    arg_type type;
    arg_value value;
} pos_c_argument;


typedef struct {
    char* arg1_name;
    char* arg2_name;
    arg_rule rule;
} arg_rule_t;


char* program_dsc = NULL;
char* program_name = NULL;
char* program_title = NULL;


opt_c_argument optional_arguments[MAX_OPT_ARGUMENTS] = {
        { .short_name = "h", .long_name = "help", .type = ARG_BOOL, .value = (arg_value) { .b = FALSE }, .help = "Show this help message" },
};
pos_c_argument positional_arguments[MAX_POS_ARGUMENTS];
char opt_arguments_count = 1;
char pos_arguments_count = 0;

arg_rule_t rules[MAX_ARG_RULES];
char rules_count = 0;

/**
 * @brief Add an argument to the parser
 * @param short_name    Short name of the argument (e.g. "d") do not use the '-' character
 * @param long_name     Long name of the argument (e.g. "debug") do not use the '--' characters
 * @param type          Type of the argument any of: ARG_INT, ARG_FLOAT, ARG_STRING, ARG_BOOL
 * @param default_value Default value of the argument (if type is 'bool', then if the argument is used it will invert the default value)
 * @param help          Help message to display when --help is used
 * @return              Amount of existing optional arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
 */
static int _ap_add_opt_argument(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help);


/**
 * @brief Add a positional argument to the parser \n
 * The order of positional arguments is important
 * @param name          Name of the argument
 * @param type          Expected type of the argument any of: ARG_INT, ARG_FLOAT, ARG_STRING
 * @param help          Help message to display when --help is used
 * @return              Amount of existing positional arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
 */
static int _ap_add_pos_argument(char* name, arg_type type, char* help);


/**
 * @brief Get the value of an argument
 * @param name  Name of the argument
 * @return      Value of the argument
 */
static arg_value _arg(char* name);


/**
 * @brief Parse the arguments
 * @param argc  Number of arguments
 * @param argv  Array of arguments
 */
static void _ap_parse_args(int argc, char* argv[]);


/**
 * @brief Set the description/help message, it will be part of the help message
 * @param dsc  Description of the program
 */
static void _ap_set_program_description(char* dsc);


/**
 * @brief Set the program name, it will be used in the help message
 * @param name  Program name
 */
static void _ap_set_program_title(char* name);


/**
 * @brief Print the help message
 */
static void _ap_print_help();


/**
 * @brief Creates new rule for input arguments\n
 * Only optional arguments can have rules assigned!
 * @param arg1  First argument
 * @param rule  Rule type:\n
 * ARG_EXCLUSIVE: arg1 and arg2 cannot be used together\n
 * ARG_REQUIRES: arg1 requires arg2 to be used
 * @param arg2  Second argument
 */
static void _ap_set_rule(char* arg1, arg_rule rule, char* arg2);

// Private function to find index of an optional argument by name
static int find_opt_argument(char* name);

// Private function to find index of a positional argument by name
static int find_pos_argument(char* name);

// Private function to check if an argument exists
static int exists(char* s_name, char* l_name);

// Skips the first dash(es) and returns only the name
static char* extract_name(char* arg);

// Prints the usage of arguments the program
static void print_arguments_usage();

static void print_rules();

// helper for printing expected argument type
static char* type_switch(arg_type type);

// After parsing the arguments, checks if the rules are satisfied
// Returns index of a broken rule or -1 if no rule is broken
static int check_rules();


static int check_rules()
{
    for(char i = 0; i < rules_count; i++)
    {
        arg_rule_t rule = rules[i];
        int arg1_index = find_opt_argument(rule.arg1_name);
        int arg2_index = find_opt_argument(rule.arg2_name);
        if (arg1_index == -1)
        {
            fprintf(stderr, "|WRN| check_rules: Unknown argument, skipping %s\n", rule.arg1_name);
            continue;
        }
        if (arg2_index == -1)
        {
            fprintf(stderr, "|WRN| check_rules: Unknown argument, skipping %s\n", rule.arg2_name);
            continue;
        }
        if (rule.rule == ARG_EXCLUSIVE)
        {
            if (optional_arguments[arg1_index].used && optional_arguments[arg2_index].used)
            {
                return i;
            }
        }
        else if (rule.rule == ARG_REQUIRES)
        {
            if (optional_arguments[arg1_index].used && !optional_arguments[arg2_index].used)
            {
                return i;
            }
        }
    }
    return -1;
}


static int exists(char* s_name, char* l_name)
{
    if (find_opt_argument(s_name) != -1 ||
        find_opt_argument(l_name) != -1 ||
        find_pos_argument(s_name) != -1 ||
        find_pos_argument(l_name) != -1)
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

static char* type_switch(arg_type type)
{
    switch (type)
    {
        case ARG_BOOL:
            return "flag";
        case ARG_INT:
            return "int";
        case ARG_STRING:
            return "string";
        case ARG_FLOAT:
            return "float";
        default:
            return "unknown";
    }
}

static void print_arguments_usage()
{
    char* local_program_name = program_name == NULL ? "program_name" : program_name;
    printf("Usage: %s ", local_program_name);
    for (int i = 0; i < opt_arguments_count; i++)
    {
        if(optional_arguments[i].type == ARG_BOOL)
        {
            printf("[-%s] ", optional_arguments[i].short_name);
        }
        else
        {
            printf("[-%s VALUE] ", optional_arguments[i].short_name);
        }
    }
    for (int i = 0; i < pos_arguments_count; i++)
    {
        printf("%s ", positional_arguments[i].name);
    }
    printf("\n");

    printf("Arguments:\n");
    for (int i = 0; i < pos_arguments_count; i++)
    {
        printf("  %s", positional_arguments[i].name);
        printf("\n\t(%s)\n\t%s\n\n", type_switch(positional_arguments[i].type), positional_arguments[i].help);
    }
    for (int i = 0; i < opt_arguments_count; i++)
    {
        printf("  -%s, --%s", optional_arguments[i].short_name, optional_arguments[i].long_name);
        if (optional_arguments[i].type != ARG_BOOL)
        {
            printf(" VALUE");
        }
        printf("\n\t(%s)", type_switch(optional_arguments[i].type));
        printf("\n");
        printf("\t%s\n\n", optional_arguments[i].help);
    }
}

static void print_rules()
{
    printf("Rules:\n");
    for (int i = 0; i < rules_count; i++)
    {
        printf("\t'%s' %s '%s'\n\n", rules[i].arg1_name, rules[i].rule == ARG_EXCLUSIVE ? "EXCLUSIVE" : "REQUIRES", rules[i].arg2_name);
    }
}

static int _ap_add_opt_argument(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help)
{
    if (opt_arguments_count >= MAX_OPT_ARGUMENTS || exists(short_name, long_name) == TRUE)    // explicit comparison with TRUE in case TRUE is defined as 0
    {
        return -1;
    }

    opt_c_argument argument;
    argument.short_name = short_name;
    argument.long_name = long_name;
    argument.type = type;
    argument.value = default_value;
    argument.help = help;
    argument.used = FALSE;
    optional_arguments[opt_arguments_count] = argument;
    opt_arguments_count++;

    return opt_arguments_count;
}


static int _ap_add_pos_argument(char* name, arg_type type, char* help)
{
    if (pos_arguments_count >= MAX_POS_ARGUMENTS || exists(name, name) == TRUE)
    {
        return -1;
    }

    pos_c_argument argument;
    argument.name = name;
    argument.help = help;
    argument.type = type;
    argument.value = (arg_value) { .s = NULL };
    positional_arguments[pos_arguments_count] = argument;
    pos_arguments_count++;

    return pos_arguments_count;
}


static arg_value _arg(char* name)
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


static void _ap_parse_args(int argc, char* argv[])
{
    program_name = argv[0];
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
                print_arguments_usage();
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
                    print_arguments_usage();
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
            optional_arguments[index].used = TRUE;
        }
        else
        {
            if (current_pos_arg >= pos_arguments_count)
            {
                fprintf(stderr, "Too many positional arguments\n");
                print_arguments_usage();
                exit(ARG_ERROR_CODE);
            }
            char* value = current_word;
            if (positional_arguments[current_pos_arg].type == ARG_INT)
            {
                positional_arguments[current_pos_arg].value.i = atoi(value);
            }
            else if (positional_arguments[current_pos_arg].type == ARG_FLOAT)
            {
                positional_arguments[current_pos_arg].value.f = atof(value);
            }
            else if (positional_arguments[current_pos_arg].type == ARG_STRING)
            {
                positional_arguments[current_pos_arg].value.s = value;
            }
            current_pos_arg++;
        }
        arg_idx++;
    }

    if (_arg("help").b == TRUE)
    {
        _ap_print_help();
        exit(0);
    }

    if (current_pos_arg < pos_arguments_count)
    {
        fprintf(stderr, "Not enough positional arguments\n");
        print_arguments_usage();
        exit(ARG_ERROR_CODE);
    }

    int rule_error = check_rules();
    if (rule_error != -1)
    {
        fprintf(stderr, "Rules not satisfied\n");
        print_arguments_usage();
        exit(ARG_ERROR_CODE);
    }
}


static void _ap_set_program_description(char* dsc)
{
    program_dsc = dsc;
}


static void _ap_set_program_title(char* name)
{
    program_title = name;
}


static void _ap_print_help()
{
    char* local_program_title = program_title == NULL ? (program_name == NULL ? "<program_name not specified>" : program_name) : program_title;
    char* local_program_dsc = program_dsc == NULL ? "<No description>" : program_dsc;

    printf("%s\n", local_program_title);
    printf("%s\n", local_program_dsc);
    print_arguments_usage();
    print_rules();
}


static void _ap_set_rule(char* arg1, arg_rule rule, char* arg2)
{
    if (rules_count >= MAX_ARG_RULES)
    {
        fprintf(stderr, "Too many rules\n");
        exit(ARG_ERROR_CODE);
    }
    rules[rules_count].arg1_name = arg1;
    rules[rules_count].rule = rule;
    rules[rules_count].arg2_name = arg2;
    rules_count++;
}


Argparser Argparser_new()
{
    Argparser ap;
    ap.add_opt_argument = _ap_add_opt_argument;
    ap.add_pos_argument = _ap_add_pos_argument;
    ap.arg = _arg;
    ap.parse_args = _ap_parse_args;
    ap.set_program_description = _ap_set_program_description;
    ap.set_program_title = _ap_set_program_title;
    ap.print_help = _ap_print_help;
    ap.set_rule = _ap_set_rule;
    return ap;
}