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

#ifndef FIELDPRINTER__H
#define FIELDPRINTER__H

#include <stdbool.h>

#include "stringlist.h"

struct fieldprinter_s;
typedef struct fieldprinter_s FIELDPRINTER;

extern FIELDPRINTER *fieldprinter_new(STRINGLIST *, const char *,
                                      const char *);
extern void fieldprinter_delete(FIELDPRINTER *);
extern void fieldprinter_print(FIELDPRINTER *, const STRINGLIST *);

#endif
