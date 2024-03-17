#ifndef C_ARGPARSER_ARGPARSER_H
#define C_ARGPARSER_ARGPARSER_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
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
 * @brief Add an argument to the parser
 * @param short_name    Short name of the argument (e.g. "-d")
 * @param long_name     Long name of the argument (e.g. "--debug")
 * @param type          Type of the argument any of: ARG_INT, ARG_FLOAT, ARG_STRING, ARG_BOOL
 * @param default_value Default value of the argument (if type is 'bool', then if the argument is used it will invert the default value)
 * @param help          Help message to display when --help is used
 * @return              Amount of existing arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
 */
int add_optional_argument(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help);


/**
 * @brief Add a positional argument to the parser \n
 * The order of positional arguments is important
 * @param name          Name of the argument
 * @param help          Help message to display when --help is used
 * @return              Amount of existing arguments (after adding) or -1 if the maximum is reached 127 or -1 if argument already exists
 */
int add_positional_argument(char* name, char* help);


/**
 * @brief Get the value of an argument
 * @param name  Name of the argument
 * @return      Value of the argument
 */
arg_value arg(char* name);


/**
 * @brief Parse the arguments
 * @param argc  Number of arguments
 * @param argv  Array of arguments
 */
void parse_args(int argc, char* argv[]);

#endif //C_ARGPARSER_ARGPARSER_H
