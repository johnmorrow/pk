/* 
 *  Copyright 2012 John Morrow
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <errno.h>
#include <error.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "stringlist.h"
#include "tokenizer.h"
#include "wrappers.h"

struct field_request_s
{
    union
    {
        const char *string;
        long int number;
    };
    enum
    { STRING, NUMBER } which;
};

static struct field_request_s convert_str_to_field(const char *str)
{
    struct field_request_s retval;
    const int base = 10;
    char *endptr;
    errno = 0;
    const long val = strtol(str, &endptr, base);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)
        || (endptr == str) || (val < 0) || (*endptr != '\0'))
    {
        retval.string = str;
        retval.which = STRING;
    }
    else
    {
        retval.number = val;
        retval.which = NUMBER;
    }
    return retval;
}

static void print_fields(const STRINGLIST *tokens,
                         const CONFIGURATION *configuration)
{
    const char *str;
    long int token_index;
    int i = 0;
    while (configuration->fields[i])
    {
        struct field_request_s fr;
        fr = convert_str_to_field(configuration->fields[i]);
        switch (fr.which)
        {
        case STRING:
            str = fr.string;
            break;
        case NUMBER:
            token_index = fr.number - 1;
            if (token_index > ((long int)stringlist_size(tokens) - 1) || token_index < 0
                || strcmp(stringlist_string(tokens, token_index), "") == 0)
            {
                str = "NULL";
            }
            else
            {
                str = stringlist_string(tokens, token_index);
            }
            break;
        }
        if (i++ == 0)
        {
            (void)printf("%s", str);
        }
        else
        {
            (void)printf("%s%s", configuration->separator, str);
        }
    }
    (void)puts("");
}

static void process_stream(FILE * stream,
                           const CONFIGURATION *configuration,
                           TOKENIZER * tokenizer)
{
    size_t allocated_bytes = 128; // Starting value, can be modified by getline.
    char *line = MALLOC_ARRAY(allocated_bytes, char);
    while (Getline(&line, &allocated_bytes, stream) != -1)
    {
        STRINGLIST *tokens = tokenizer_create_tokens(tokenizer, line);
        print_fields(tokens, configuration);
        tokenizer_free_tokens(tokenizer);
    }
    free(line);
}

static FILE *open_input(const char *file)
{
    FILE *input;
    if (file)
    {
        (void)fclose(stdin);
        input = Fopen(file, "r");
    }
    else
    {
        input = stdin;
    }
    setlinebuf(input);
    return input;
}

int main(int argc, char **argv)
{
    setlinebuf(stdout);
    CONFIGURATION *configuration = configuration_new(argc, argv);
    TOKENIZER *tokenizer = tokenizer_new();
    tokenizer_allow_empty_tokens(tokenizer, configuration->allow_empty_tokens);
    tokenizer_set_delimiters(tokenizer, configuration->delimiters);
    FILE *fp = open_input(configuration->file);
    process_stream(fp, configuration, tokenizer);
    (void)fclose(fp);
    tokenizer_delete(tokenizer);
    configuration_delete(configuration);
    exit(EXIT_SUCCESS);
}
