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
    const char **strings;
    size_t allocated;
    size_t used;
};

STRINGLIST *stringlist_new()
{
    STRINGLIST *self = MALLOC(STRINGLIST);
    self->allocated = 1;
    self->used = 0;
    self->strings = MALLOC_ARRAY(self->allocated, const char *);
    return self;
}

void stringlist_delete(STRINGLIST *self)
{
    Free(self->strings);
    Free(self);
}

void stringlist_add(STRINGLIST *self, const char *string)
{
    if (self->used == self->allocated)
    {
        self->allocated *= 2;
        self->strings =
            REALLOC_ARRAY(self->strings, self->allocated, const char *);
    }
    self->strings[self->used++] = string;
}

const char *stringlist_string(const STRINGLIST *self, size_t index)
{
    return self->strings[index];
}

size_t stringlist_size(const STRINGLIST *self)
{
    return self->used;
}

void stringlist_debug(const STRINGLIST *self)
{
    for (size_t i = 0; i < self->used; ++i)
    {
        (void)fprintf(stderr, "[%lu:%s] ", i, self->strings[i]);
    }
    (void)fprintf(stderr, "\n");
}
