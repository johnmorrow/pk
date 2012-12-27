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
#include "frequest.h"
#include "stringlist.h"
#include "tokenizer.h"
#include "wrappers.h"

static void process_stream(FILE * stream,
                           const CONFIGURATION *configuration,
                           TOKENIZER * tokenizer,
                           FREQUEST *request)
{
    size_t allocated_bytes = 128; // Starting value, can be modified by getline.
    char *line = MALLOC_ARRAY(allocated_bytes, char);
    while (Getline(&line, &allocated_bytes, stream) != -1)
    {
        STRINGLIST *tokens = tokenizer_create_tokens(tokenizer, line);
        frequest_print(request, tokens, configuration->separator);
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
    tokenizer_allow_escape_characters(tokenizer,
                                   configuration->backslash_escapes_delimiters);
    tokenizer_set_delimiters(tokenizer, configuration->delimiters);


    int i = 0;
    while (configuration->fields[i])
    {
        i++;
    }
    FREQUEST *request = frequest_new(i, configuration->fields);

    FILE *fp = open_input(configuration->file);
    process_stream(fp, configuration, tokenizer, request);
    (void)fclose(fp);
    frequest_delete(request);
    tokenizer_delete(tokenizer);
    configuration_delete(configuration);
    exit(EXIT_SUCCESS);
}
