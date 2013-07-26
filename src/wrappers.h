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

#ifndef WRAPPERS__H
#define WRAPPERS__H

#include <stdio.h>

extern void *Malloc(size_t);
extern void *Realloc(void *, size_t);
extern void Free(void *);
extern ssize_t Getline(char **, size_t *, FILE *);
extern int Asprintf(char **, const char *, ...);
extern int Fclose(FILE *);
extern FILE *Fopen(const char *, const char *);
extern char *Strdup(const char *);

/* allocates a single object using malloc(). */
#define MALLOC(type) ((type *)Malloc(sizeof(type)))

/* allocates an array of objects using malloc(). */
#define MALLOC_ARRAY(number, type) \
        ((type *)Malloc((number) * sizeof(type)))

/* reallocates an array of objects using realloc(). */
#define REALLOC_ARRAY(pointer, number, type) \
        ((type *)Realloc(pointer, (number) * sizeof(type)))

#endif
