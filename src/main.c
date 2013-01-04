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
#include "fieldprinter.h"
#include "stringlist.h"
#include "tokenizer.h"
#include "wrappers.h"

int main(int argc, char **argv)
{
    CONFIGURATION *configuration = configuration_new(argc, argv);
    FILE *input = stdin;
    if (configuration->file)
    {
        Fclose(stdin);
        input = Fopen(configuration->file, "r");
    }
    setlinebuf(input);
    setlinebuf(stdout);
    TOKENIZER *tokenizer = tokenizer_new();
    tokenizer_enable_empty_tokens(tokenizer, configuration->allow_empty_tokens);
    tokenizer_enable_escaped_delimiters(tokenizer,
                                   configuration->backslash_escapes_delimiters);
    tokenizer_enable_trimming(tokenizer, configuration->trim_non_alphanumeric);
    tokenizer_set_delimiters(tokenizer, configuration->delimiters);
    tokenizer_set_excludes(tokenizer, configuration->excludes);
    FIELDPRINTER *printer = fieldprinter_new(configuration->fields,
            configuration->separator, configuration->empty_string);
    size_t allocated_bytes = 128;  // initial value can be modified by getline.
    char *line = MALLOC_ARRAY(allocated_bytes, char);
    while (Getline(&line, &allocated_bytes, input) != -1)
    {
        STRINGLIST *tokens = tokenizer_create_tokens(tokenizer, line);
        fieldprinter_print(printer, tokens);
        tokenizer_free_tokens(tokenizer);
    }
    Free(line);
    fieldprinter_delete(printer);
    tokenizer_delete(tokenizer);
    Fclose(input);
    configuration_delete(configuration);
    exit(EXIT_SUCCESS);
}
