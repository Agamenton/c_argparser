#include "argparser.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* short_name;
    char* long_name;
    arg_type type;
    arg_value value;
    char* help;
    char optional;
} c_argument;


c_argument arguments[127];
char arguments_count = 0;


// Private function to find index of an argument by name
static int find_argument(char* name);

// Private function to check if an argument exists
static int exists(char* s_name, char* l_name);


static int exists(char* s_name, char* l_name)
{
    if (find_argument(s_name) != -1 || find_argument(l_name) != -1)
    {
        return TRUE;
    }
    return FALSE;
}


static int find_argument(char* name)
{
    for (int i = 0; i < arguments_count; i++)
    {
        if (strcmp(arguments[i].short_name, name) == 0 || strcmp(arguments[i].long_name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}


int add_optional_argument(char* short_name, char* long_name, arg_type type, arg_value default_value, char* help)
{
    if (arguments_count >= 127 || exists(short_name, long_name) == TRUE)    // explicit comparison with TRUE in case TRUE is defined as 0
    {
        return -1;
    }

    c_argument argument;
    argument.short_name = short_name;
    argument.long_name = long_name;
    argument.type = type;
    argument.value = default_value;
    argument.help = help;
    argument.optional = TRUE;
    arguments[arguments_count] = argument;
    arguments_count++;

    return arguments_count;
}


int add_positional_argument(char* name, char* help)
{
    if (arguments_count >= 127 || exists(name, name) == TRUE)
    {
        return -1;
    }

    c_argument argument;
    argument.short_name = name;
    argument.long_name = name;
    argument.type = ARG_STRING;
    argument.value.s = NULL;
    argument.help = help;
    argument.optional = FALSE;
    arguments[arguments_count] = argument;
    arguments_count++;

    return arguments_count;
}

arg_value arg(char* name)
{
    int index = find_argument(name);
    if (index == -1)
    {
        arg_value value;
        value.i = -1;
        return value;
    }
    return arguments[index].value;
}

void parse_args(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            int index = find_argument(argv[i]);
            if (index != -1)
            {
                if (arguments[index].type == ARG_BOOL)
                {
                    arguments[index].value.b = !arguments[index].value.b;
                }
                else
                {
                    if (i + 1 < argc)
                    {
                        if (arguments[index].type == ARG_INT)
                        {
                            arguments[index].value.i = atoi(argv[i + 1]);
                        }
                        else if (arguments[index].type == ARG_FLOAT)
                        {
                            arguments[index].value.f = atof(argv[i + 1]);
                        }
                        else if (arguments[index].type == ARG_STRING)
                        {
                            arguments[index].value.s = argv[i + 1];
                        }
                        i++;
                    }
                }
            }
        }
        else
        {
            for (int j = 0; j < arguments_count; j++)
            {
                if (arguments[j].optional == FALSE)
                {
                    if (arguments[j].value.s == NULL)
                    {
                        arguments[j].value.s = argv[i];
                        break;
                    }
                }
            }
        }
    }

}