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

#include "compat.h"

#ifndef HAVE_GETLINE

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ssize_t getline(char **out, size_t * outsize, FILE * fp)
{
    size_t len;
    char *buf = fgetln(fp, &len);
    if (buf == NULL)
    {
        return -1;
    }
    if (*out == NULL || *outsize < len + 1)
    {
        void *tmp = realloc(*out, len + 1);
        if (tmp == NULL)
        {
            return -1;
        }
        *out = tmp;
        *outsize = len + 1;
    }
    (void)memcpy(*out, buf, len);
    (*out)[len] = '\0';
    return len;
}

#endif
