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

#ifndef CONFIGURATION__H
#define CONFIGURATION__H

#include "stringlist.h"
#include "tokenizer.h"

typedef struct configuration_s
{
    bool allow_empty_tokens;
    bool backslash_escapes_delimiters;
    bool trim_non_alphanumeric;
    char *delimiters;
    char *file;
    const char *empty_string;
    const char *separator;
    STRINGLIST *excludes;
    STRINGLIST *fields;
    /*
     * internal state
     */
    TOKENIZER *tokenizer;
} CONFIGURATION;

extern CONFIGURATION *configuration_new(int, char **);
extern void configuration_delete(CONFIGURATION *);

#endif
