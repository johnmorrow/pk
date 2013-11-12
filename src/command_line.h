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

#ifndef COMMAND_LINE__H
#define COMMAND_LINE__H

#include <stdbool.h>

struct command_line_s;
typedef struct command_line_s COMMAND_LINE;

extern COMMAND_LINE *command_line_new();
extern void command_line_add_flag(COMMAND_LINE *, char, const char *, const char *);
extern bool command_line_check_flag(COMMAND_LINE *, char);
extern void command_line_delete(COMMAND_LINE *);
extern void command_line_parse(COMMAND_LINE *, int, const char *[]);

#endif
