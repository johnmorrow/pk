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

#ifndef TOKENIZER__H
#define TOKENIZER__H

#include <stdbool.h>

#include "stringlist.h"

struct tokenizer_s;
typedef struct tokenizer_s TOKENIZER;


extern TOKENIZER *tokenizer_new();
extern void tokenizer_delete(TOKENIZER *);

extern void tokenizer_allow_empty_tokens(TOKENIZER *, bool);
extern void tokenizer_trim_token(TOKENIZER *, bool);
extern void tokenizer_allow_escape_characters(TOKENIZER *, bool);
extern void tokenizer_allow_double_quotes(TOKENIZER *, bool);
extern void tokenizer_set_delimiters(TOKENIZER *, const char *);
extern void tokenizer_set_excludes(TOKENIZER *, const STRINGLIST *);

extern STRINGLIST *tokenizer_create_tokens(TOKENIZER *, const char *);
extern void tokenizer_free_tokens(TOKENIZER *);


#endif
