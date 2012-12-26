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

#ifndef CONFIGURATION__H
#define CONFIGURATION__H

typedef struct configuration_s
{
    const char *delimiters;
    char **exclude;
    char **fields;
    char *file;
    const char *separator;
    int allow_empty_tokens;
    int backslash_escapes_delimiters;
} CONFIGURATION;

extern CONFIGURATION *configuration_new(int, char **);
extern void configuration_delete(CONFIGURATION *);

#endif
