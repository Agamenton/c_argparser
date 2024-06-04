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

    /**
     * @brief Add an argument to the parser
     * @param short_name    Short name of the argument (e.g. "d") do not use the '-' character
     * @param long_name     Long name of the argument (e.g. "debug") do not use the '--' characters
     * @param type          Type of the argument any of: ARG_INT, ARG_FLOAT, ARG_STRING, ARG_BOOL
     * @param default_value Default value of the argument (if type is 'bool', then if the argument is used it will invert the default value)
     * @param help          Help message to display when --help is used
     * @return              Amount of existing optional arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
     */
    int (*add_opt_argument)(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help);

    /**
     * @brief Add a positional argument to the parser \n
     * The order of positional arguments is important
     * @param name          Name of the argument
     * @param type          Expected type of the argument any of: ARG_INT, ARG_FLOAT, ARG_STRING
     * @param help          Help message to display when --help is used
     * @return              Amount of existing positional arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
     */
    int (*add_pos_argument)(char* name, arg_type type, char* help);

    /**
     * @brief Get the value of an argument
     * @param name  Name of the argument
     * @return      Value of the argument
     */
    arg_value (*arg)(char* name);

    /**
     * @brief Parse the arguments
     * @param argc  Number of arguments
     * @param argv  Array of arguments
     */
    void (*parse_args)(int argc, char* argv[]);

    /**
     * @brief Set the description/help message, it will be part of the help message
     * @param dsc  Description of the program
     */
    void (*set_program_description)(char* dsc);

    /**
     * @brief Set the program name, it will be used in the help message
     * @param name  Program name
     */
    void (*set_program_title)(char* name);

    /**
     * @brief Print the help message
     */
    void (*print_help)();

    /**
     * @brief Creates new rule for input arguments\n
     * Only optional arguments can have rules assigned!
     * @param arg1  First argument
     * @param rule  Rule type:\n
     * ARG_EXCLUSIVE: arg1 and arg2 cannot be used together\n
     * ARG_REQUIRES: arg1 requires arg2 to be used
     * @param arg2  Second argument
     */
    void (*set_rule)(char* arg1, arg_rule rule, char* arg2);
} Argparser;

Argparser Argparser_new();



#endif //C_ARGPARSER_ARGPARSER_H
