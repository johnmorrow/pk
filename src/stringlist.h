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

#ifndef STRINGLIST__H
#define STRINGLIST__H

#include <stdbool.h>

struct stringlist_s;
typedef struct stringlist_s STRINGLIST;

extern STRINGLIST *stringlist_new();
extern void stringlist_delete(STRINGLIST *);
extern void stringlist_add(STRINGLIST *, const char *);
extern const char *stringlist_string(const STRINGLIST *, size_t);
extern size_t stringlist_size(const STRINGLIST *);

#endif
