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

#include <stdlib.h>
#include <string.h>

#include "stringlist.h"
#include "wrappers.h"

struct stringlist_s
{
    char **strings;
    size_t allocated;
    size_t used;
    bool internal_allocation;
};

STRINGLIST *stringlist_new()
{
    STRINGLIST *self = MALLOC(STRINGLIST);
    self->allocated = 1;
    self->used = 0;
    self->strings = MALLOC_ARRAY(self->allocated, char *);
    self->internal_allocation = false;
    return self;
}

void stringlist_delete(STRINGLIST * self)
{
    if (self->internal_allocation)
    {
        for (size_t i = 0; i < self->used; ++i)
        {
            if (self->strings[i])
            {
                Free(self->strings[i]);
            }
        }
    }
    Free(self->strings);
    Free(self);
}

STRINGLIST *stringlist_copy(const STRINGLIST * self)
{
    STRINGLIST *copy = MALLOC(STRINGLIST);
    copy->allocated = self->allocated;
    copy->used = self->used;
    copy->strings = MALLOC_ARRAY(copy->allocated, char *);
    copy->internal_allocation = true;
    for (size_t i = 0; i < self->used; ++i)
    {
        if (self->strings[i])
        {
            copy->strings[i] = Strdup(self->strings[i]);    // Deep copy
        }
        else
        {
            copy->strings[i] = NULL;
        }
    }
    return copy;
}

void stringlist_add(STRINGLIST * self, char *string)
{
    if (self->used == self->allocated)
    {
        self->allocated *= 2;
        self->strings = REALLOC_ARRAY(self->strings, self->allocated, char *);
    }
    if (self->internal_allocation)
    {
        self->strings[self->used++] = Strdup(string);
    }
    else
    {
        self->strings[self->used++] = string;
    }
}

const char *stringlist_string(const STRINGLIST * self, size_t index)
{
    return self->strings[index];
}

size_t stringlist_size(const STRINGLIST * self)
{
    return self->used;
}

void stringlist_debug(const STRINGLIST * self)
{
    for (size_t i = 0; i < self->used; ++i)
    {
        (void)fprintf(stderr, "[%lu:%s] ", (unsigned long)i, self->strings[i]);
    }
    (void)fprintf(stderr, "\n");
}
