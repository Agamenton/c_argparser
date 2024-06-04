#ifndef C_ARGPARSER_ARGPARSER_H
#define C_ARGPARSER_ARGPARSER_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ARG_ERROR_CODE
#define ARG_ERROR_CODE 55
#endif

#ifndef MAX_OPT_ARGUMENTS
#define MAX_OPT_ARGUMENTS 127
#endif

#ifndef MAX_POS_ARGUMENTS
#define MAX_POS_ARGUMENTS 127
#endif

#ifndef MAX_ARG_RULES
#define MAX_ARG_RULES 127
#endif


/**
 * @brief Type of the argument used when adding an argument to the parser
 *
 * ARG_INT:    Integer: example -d 10
 * ARG_FLOAT:  Float: example -f 10.5
 * ARG_STRING: String: example -s "Hi!"
 * ARG_BOOL:   Boolean: example -b
 */
typedef enum {
    ARG_INT,
    ARG_FLOAT,
    ARG_STRING,
    ARG_BOOL
} arg_type;


/**
 * @brief Used to make the return value of the arg getter dynamic
 */
typedef union {
    int i;
    float f;
    char* s;
    int b;
} arg_value;


/**
 * @brief Used to specify the rule type of new rule
 */
typedef enum {
    ARG_EXCLUSIVE,
    ARG_REQUIRES
} arg_rule;

// Version 2.0, use struct to call all functions (as if it was a class)
typedef struct {
    int (*add_opt_argument)(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help);
    int (*add_pos_argument)(char* name, arg_type type, char* help);
    arg_value (*arg)(char* name);
    void (*parse_args)(int argc, char* argv[]);
    void (*set_program_description)(char* dsc);
    void (*set_program_title)(char* name);
    void (*print_help)();
    void (*set_rule)(char* arg1, arg_rule rule, char* arg2);
} Argparser;

Argparser Argparser_new();



#endif //C_ARGPARSER_ARGPARSER_H
