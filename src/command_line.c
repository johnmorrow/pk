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

#include "command_line.h"
#include "wrappers.h"

typedef enum {
  ARGUMENT_FLAG,
  ARGUMENT_PARAMETER,
} argument_types;

typedef struct command_option_s
{
    argument_types argument_type;
} COMMAND_OPTION;

static COMMAND_OPTION *command_option_new(char alphanumeric_id,
    const char *long_id, argument_types argument_type,
    const char *short_documentation_string)
{
    COMMAND_OPTION *self = MALLOC(COMMAND_OPTION);
    return self;
}

static void command_option_delete(COMMAND_OPTION * self)
{
    Free(self);
}

struct command_line_s
{
    COMMAND_OPTION **options;
    size_t options_allocated;
    size_t options_used;
};

COMMAND_LINE *command_line_new()
{
    const int preallocate_space_for_N_options = 10;
    COMMAND_LINE *self = MALLOC(COMMAND_LINE);
    self->options_allocated = preallocate_space_for_N_options;
    self->options_used = 0;
    self->options = MALLOC_ARRAY(self->options_allocated, COMMAND_OPTION *);
    return self;
}

void command_line_add_flag(COMMAND_LINE *self, char alphanumeric_id,
    const char *long_id, const char *short_documentation_string)
{
    if (self->options_used == self->options_allocated)
    {
        self->options_allocated *= 2;
        self->options = REALLOC_ARRAY(self->options, self->options_allocated,
                                      COMMAND_OPTION *);
    }
    COMMAND_OPTION *option = command_option_new(alphanumeric_id, long_id,
        ARGUMENT_FLAG, short_documentation_string);
    self->options[self->options_used++] = option;
}

bool command_line_check_flag(COMMAND_LINE *self, char alphanumeric_id)
{
    return true;
}

void command_line_delete(COMMAND_LINE *self)
{
    for (size_t i = 0; i < self->options_used; ++i)
    {
        if (self->options[i])
        {
            command_option_delete(self->options[i]);
        }
    }
    Free(self->options);
    Free(self);
}

void command_line_parse(COMMAND_LINE *self, int argc, const char *argv[])
{
    for (int i = 0; i < 0; ++i)
    {
        if (! is_an_option(argv[1])
        {
        }
    }
}
