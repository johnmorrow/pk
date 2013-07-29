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

#include <err.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "fieldprinter.h"
#include "helpers.h"
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
    } u;
    enum
    { NUMBER, STRING, RANGE } which;
};

struct fieldprinter_s
{
    struct field_s **fields;
    size_t field_count;
    const STRINGLIST *excludes;
    const char *separator;
    const char *empty_string;
    bool at_start;
};

static struct field_s *field_number(size_t number)
{
    struct field_s *retval = MALLOC(struct field_s);
    retval->which = NUMBER;
    retval->u.number = number;
    return retval;
}

static struct field_s *field_range(size_t start, size_t finish)
{
    struct field_s *retval = MALLOC(struct field_s);
    retval->which = RANGE;
    retval->u.range.start = start;
    retval->u.range.finish = finish;
    return retval;
}

static struct field_s *field_string(const char *str)
{
    struct field_s *retval = MALLOC(struct field_s);
    retval->which = STRING;
    retval->u.string = str;
    return retval;
}

static bool is_long(register const char *str, size_t len, size_t start,
                    size_t * finish, register unsigned long *number)
{
    register unsigned int pos = start;
    register unsigned long result = 0;
    register unsigned long c;
    if (start >= len)
    {
        return false;
    }
    while ((c = (unsigned long)(unsigned char)(str[pos] - '0')) < 10)
    {
        result = result * 10 + c;
        ++pos;
    }
    *number = result;
    *finish = pos;
    return pos != 0;
}

static bool is_range(const char *str, size_t len, size_t start,
                     size_t * finish)
{
    if (start >= len)
    {
        return false;
    }
    if (str[start] == '.' && str[start + 1] == '.')
    {
        *finish = start + 2;
        return true;
    }
    return false;
}

static bool is_end(const char *str, size_t len, size_t start, size_t * finish)
{
    if (start >= len)
    {
        return true;
    }
    return false;
}

static struct field_s *str_to_field(const char *str)
{
    unsigned long end;
    unsigned long start;
    size_t at = 0;
    size_t len = strlen(str);
    if (is_long(str, len, at, &at, &start))
    {
        if (is_range(str, len, at, &at))
        {
            if (is_long(str, len, at, &at, &end))
            {
                if (is_end(str, len, at, &at))
                {
                    return field_range(start, end);
                }
            }
            else if (is_end(str, len, at, &at))
            {
                return field_range(start, 0);
            }
        }
        else if (is_end(str, len, at, &at))
        {
            return field_number(start);
        }
    }
    else if (is_range(str, len, at, &at))
    {
        if (is_long(str, len, at, &at, &end))
        {
            if (is_end(str, len, at, &at))
            {
                return field_range(1, end);
            }
        }
        else if (is_end(str, len, at, &at))
        {
            return field_range(1, 0);
        }
    }
    return field_string(str);
}

FIELDPRINTER *fieldprinter_new(STRINGLIST * fields, const char *separator,
                               const char *empty_string)
{
    FIELDPRINTER *self = MALLOC(FIELDPRINTER);
    self->field_count = stringlist_size(fields);
    self->fields = MALLOC_ARRAY(self->field_count, struct field_s *);
    for (size_t i = 0; i < self->field_count; ++i)
    {
        self->fields[i] = str_to_field(stringlist_string(fields, i));
    }
    self->separator = separator;
    self->empty_string = empty_string;
    self->at_start = true;
    return self;
}

void fieldprinter_delete(FIELDPRINTER * self)
{
    for (size_t i = 0; i < self->field_count; ++i)
    {
        Free(self->fields[i]);
    }
    Free(self->fields);
    Free(self);
}

static void output_string(FIELDPRINTER * self, const char *string)
{
    if (string == NULL || *string == '\0')
    {
        string = self->empty_string;
    }
    if (string)
    {
        if (self->at_start)
        {
            (void)printf("%s", string);
            self->at_start = false;
        }
        else
        {
            (void)printf("%s%s", self->separator, string);
        }
    }
}

static void output_field(FIELDPRINTER * self, const STRINGLIST * tokens,
                         size_t token_index)
{
    const char *token;
    if (token_index >= stringlist_size(tokens)
        || (token = stringlist_string(tokens, token_index)) == NULL
        || strcmp(token, "") == 0)
    {
        output_string(self, NULL);
    }
    else
    {
        output_string(self, token);
    }
}

void fieldprinter_print(FIELDPRINTER * self, const STRINGLIST * tokens)
{
    size_t range_index_start;
    size_t range_index_finish;
    size_t token_index;
    size_t field_index = 0;
    self->at_start = true;
    while (field_index < self->field_count)
    {
        struct field_s *f = self->fields[field_index];
        switch (f->which)
        {
        case STRING:
            output_string(self, f->u.string);
            break;
        case RANGE:
            range_index_start = Position_to_index(f->u.range.start);
            if (f->u.range.finish == 0)
            {
                range_index_finish =
                    Position_to_index(stringlist_size(tokens));
            }
            else
            {
                range_index_finish = Position_to_index(f->u.range.finish);
            }
            for (size_t i = range_index_start; i <= range_index_finish; i++)
            {
                output_field(self, tokens, i);
            }
            break;
        case NUMBER:
            token_index = Position_to_index(f->u.number);
            output_field(self, tokens, token_index);
            break;
        }
        field_index += 1;
    }
    (void)puts("");
}
