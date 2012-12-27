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
#include <stdlib.h>
#include <string.h>

#include "frequest.h"
#include "wrappers.h"

struct field_s
{
    union
    {
        const char *string;
        long int number;
        struct range_s
        {
            long int start;
            long int finish;
        } range;
    };
    enum
    { NUMBER, STRING, RANGE } which;
};

struct frequest_s
{
    struct field_s **fields;
    size_t field_count;
};

static struct field_s *str_to_field(const char *str)
{
    char *tmp1 = NULL;
    char *tmp2 = NULL;
    struct field_s *retval = MALLOC(struct field_s);
    if (Sscanf(str, "%zu-%zu", &retval->range.start, &retval->range.finish) ==
        2)
    {
        retval->which = RANGE;
    }
    else if (Sscanf(str, "%zu%a[-]", &retval->range.start, &tmp1) == 2)
    {
        retval->range.finish = 0;
        retval->which = RANGE;
    }
    else if (Sscanf(str, "%a[-]%zu", &tmp2, &retval->range.finish) == 2)
    {
        retval->range.start = 1;
        retval->which = RANGE;
    }
    else if (strcmp(str, "-") == 0)
    {
        retval->range.start = 1;
        retval->range.finish = 0;
        retval->which = RANGE;
    }
    else if (Sscanf(str, "%zu", &retval->number) == 1)
    {
        retval->which = NUMBER;
    }
    else
    {
        retval->string = str;
        retval->which = STRING;
    }
    if (tmp1)
    {
        Free(tmp1);
    }
    if (tmp2)
    {
        Free(tmp2);
    }
    return retval;
}

FREQUEST *frequest_new(int argc, char **argv)
{
    FREQUEST *self = MALLOC(FREQUEST);
    self->field_count = argc;
    self->fields = MALLOC_ARRAY(self->field_count, struct field_s *);
    for (size_t i = 0; i < self->field_count; ++i)
    {
        self->fields[i] = str_to_field(argv[i]);
    }
    return self;
}

void frequest_delete(FREQUEST *self)
{
    for (size_t i = 0; i < self->field_count; ++i)
    {
        Free(self->fields[i]);
    }
    Free(self->fields);
    Free(self);
}

static void output_string(const char *str, const char *separator, bool first)
{
    if (first)
    {
        (void)printf("%s", str);
    }
    else
    {
        (void)printf("%s%s", separator, str);
    }
}

static void output_field(const FREQUEST *self, const STRINGLIST *tokens,
                         const char *separator, long int token_index,
                         bool first)
{
    const char *token;
    long int maximum_token_index = stringlist_size(tokens) - 1;
    if (token_index > maximum_token_index || token_index < 0
        || (token = stringlist_string(tokens, token_index)) == NULL
        || strcmp(token, "") == 0)
    {
        output_string("NULL", separator, first);
    }
    else
    {
        output_string(token, separator, first);
    }
}

void frequest_print(const FREQUEST *self, const STRINGLIST *tokens,
                    const char *separator)
{
    long int token_index;
    size_t field_index = 0;
    bool first = true;
    while (field_index < self->field_count)
    {
        struct field_s *f = self->fields[field_index];
        switch (f->which)
        {
        case STRING:
            output_string(f->string, separator, first);
            first = false;
            break;
        case RANGE:
            if (f->range.finish == 0)
            {
                f->range.finish = stringlist_size(tokens);
            }
            for (long int i = f->range.start - 1; i < f->range.finish; i++)
            {
                output_field(self, tokens, separator, i, first);
                first = false;
            }
            break;
        case NUMBER:
            token_index = f->number - 1;
            output_field(self, tokens, separator, token_index, first);
            first = false;
            break;
        }
        field_index += 1;
    }
    (void)puts("");
}
