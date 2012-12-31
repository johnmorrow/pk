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
        size_t number;
        struct range_s
        {
            size_t start;
            size_t finish;
        } range;
    };
    enum
    { NUMBER, STRING, RANGE } which;
};

struct frequest_s
{
    struct field_s **fields;
    size_t field_count;
    const STRINGLIST *excludes;
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

FREQUEST *frequest_new(STRINGLIST *fields)
{
    FREQUEST *self = MALLOC(FREQUEST);
    self->field_count = stringlist_size(fields);
    self->fields = MALLOC_ARRAY(self->field_count, struct field_s *);
    for (size_t i = 0; i < self->field_count; ++i)
    {
        self->fields[i] = str_to_field(stringlist_string(fields, i));
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

static bool output_string(const char *string, const char *separator, bool first)
{
    bool printed = false;
    if (string && *string != '\0')
    {
        if (first)
        {
            (void)printf("%s", string);
        }
        else
        {
            (void)printf("%s%s", separator, string);
        }
        printed = true;
    }
    return printed;
}

static bool output_field(const FREQUEST *self, const STRINGLIST *tokens,
                         const char *separator, const char *empty_string,
                         size_t token_index, bool first )
{
    bool printed = false;
    const char *token;
    if (token_index >= stringlist_size(tokens)
        || (token = stringlist_string(tokens, token_index)) == NULL
        || strcmp(token, "") == 0)
    {
        printed = output_string(empty_string, separator, first);
    }
    else
    {
        printed = output_string(token, separator, first);
    }
    return printed;
}

void frequest_print(const FREQUEST *self, const STRINGLIST *tokens,
                    const char *separator, const char *empty_string)
{
    size_t range_index_start;
    size_t range_index_finish;
    size_t token_index;
    size_t field_index = 0;
    bool first = true;
    while (field_index < self->field_count)
    {
        struct field_s *f = self->fields[field_index];
        switch (f->which)
        {
        case STRING:
            if (output_string(f->string, separator, first))
            {
                first = false;
            }
            break;
        case RANGE:
            range_index_start = Position_to_index(f->range.start);
            if (f->range.finish == 0)
            {
                range_index_finish = Position_to_index(stringlist_size(tokens));
            }
            else
            {
                range_index_finish = Position_to_index(f->range.finish);
            }
            for (size_t i = range_index_start; i <= range_index_finish; i++)
            {
                if (output_field(self, tokens, separator, empty_string, i, first))
                {
                    first = false;
                }
            }
            break;
        case NUMBER:
            token_index = Position_to_index(f->number);
            if (output_field(self, tokens, separator, empty_string, token_index,
                         first))
            {
                first = false;
            }
            break;
        }
        field_index += 1;
    }
    (void)puts("");
}
