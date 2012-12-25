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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrappers.h"

void *Malloc(size_t size)
{
    errno = 0;
    void *memory = malloc(size);
    if (memory == NULL && errno != 0)
    {
        error(EXIT_FAILURE, errno, "unable to allocate memory");
    }
    return memory;
}

void *Realloc(void *ptr, size_t size)
{
    errno = 0;
    void *memory = realloc(ptr, size);
    if (memory == NULL && errno != 0)
    {
        error(EXIT_FAILURE, errno, "unable to allocate memory");
    }
    return memory;
}

void Free(void *ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = NULL;
    }
    else
    {
        error(EXIT_FAILURE, errno, "null pointer passed to free");
    }
}

ssize_t Getline(char **lineptr, size_t * n, FILE * stream)
{
    errno = 0;
    ssize_t bytes_read = getline(lineptr, n, stream);
    if (bytes_read == -1 && errno != 0)
    {
        error(EXIT_FAILURE, errno, "error reading input");
    }
    return bytes_read;
}

int Asprintf(char **strp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    errno = 0;
    int retval = vasprintf(strp, fmt, ap);
    va_end(ap);
    if (retval == -1)
    {
        error(EXIT_FAILURE, errno, "unable to allocate memory");
    }
    return retval;
}

FILE *Fopen(const char *path, const char *mode)
{
    errno = 0;
    FILE *fp = fopen(path, mode);
    if (fp == NULL)
    {
        error(EXIT_FAILURE, errno, "%s", path);
    }
    return fp;
}

char *Strdup(const char *s)
{
    char *copy;
    errno = 0;
    copy = strdup(s);
    if (!copy)
    {
        error(EXIT_FAILURE, errno, "unable to duplicate string");
    }
    return copy;
}
