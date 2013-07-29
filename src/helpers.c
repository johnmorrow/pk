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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "wrappers.h"

char *convert_escaped_delimiters(const char *input)
{
    char *output = Malloc(strlen(input) + 1);
    bool escaped = false;
    size_t op = 0;
    for (size_t ip = 0; input[ip]; ++ip)
    {
        if (!escaped && input[ip] == '\\')
        {
            escaped = true;
        }
        else
        {
            if (escaped)
            {
                switch (input[ip])
                {
                case 't':
                    output[op] = '\t';
                    break;
                case 'f':
                    output[op] = '\f';
                    break;
                case 'n':
                    output[op] = '\n';
                    break;
                case 'r':
                    output[op] = '\r';
                    break;
                case 'v':
                    output[op] = '\v';
                    break;
                default:
                    output[op] = input[ip];
                    break;
                }
            }
            else
            {
                output[op] = input[ip];
            }
            op += 1;
            escaped = false;
        }
    }
    output[op] = '\0';
    return output;
}

size_t Position_to_index(size_t position)
{
    if (position == 0)
    {
        errx(EXIT_FAILURE, "offset bug detected");
    }
    return position - 1;
}
