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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "stringlist.h"
#include "wrappers.h"

struct tokenizer_s
{
    /*
     * These fields affect how the tokenizer operates.
     */
    bool allow_empty_tokens;
    bool trim_token;
    bool allow_escape_characters;
    bool allow_double_quotes;
    const char *delimiters;
    const STRINGLIST *excludes;
    char escape_character;
    /*
     * These fields store internal state.
     */
    const char *original;
    char *copy;
    char *strsep_ptr;
    STRINGLIST *tokens;
    char *current_token;
};

TOKENIZER *tokenizer_new()
{
    TOKENIZER *self = MALLOC(TOKENIZER);
    self->delimiters = " \t";
    self->escape_character = '\\';
    self->allow_empty_tokens = false;
    self->trim_token = false;
    self->allow_escape_characters = false;
    self->allow_double_quotes = false;
    self->excludes = NULL;
    self->original = NULL;
    self->copy = NULL;
    self->strsep_ptr = NULL;
    self->current_token = NULL;
    return self;
}

void tokenizer_delete(TOKENIZER *self)
{
    Free(self);
}

void tokenizer_enable_empty_tokens(TOKENIZER *self, bool yesno)
{
    self->allow_empty_tokens = yesno;
}

void tokenizer_enable_trimming(TOKENIZER *self, bool yesno)
{
    self->trim_token = yesno;
}

void tokenizer_enable_escaped_delimiters(TOKENIZER *self, bool yesno)
{
    self->allow_escape_characters = yesno;
}

void tokenizer_enable_double_quotes(TOKENIZER *self, bool yesno)
{
    self->allow_double_quotes = yesno;
}

void tokenizer_set_delimiters(TOKENIZER *self, const char *delimiters)
{
    self->delimiters = delimiters;
}

void tokenizer_set_excludes(TOKENIZER *self, const STRINGLIST *excludes)
{
    self->excludes = excludes;
}

static void remove_line_ending(char *line)
{
    const size_t length = strlen(line);
    size_t end_index = Position_to_index(length);
    while (line[end_index] == '\n' || line[end_index] == '\r')
    {
        line[end_index] = '\0';
        end_index -= 1;
    }
}

static void remove_string(char *string, const char *remove)
{
    char *found;
    size_t length_remove = strlen(remove);
    while((found = strstr(string, remove)) != NULL)
    {
        size_t remaining = strlen(found + length_remove) + 1;
        (void)memmove(found, found + length_remove, remaining);
    }
}

static void remove_escape_character(char *string, char escape_character)
{
    char *found;
    while((found = strchr(string, escape_character)) != NULL)
    {
        (void)memmove(found, found+1, strlen(found));
        /* Handle double escape character by skipping forward */
        if (found && *found == escape_character)
        {
            string = found + 1;
        }
    }
}

static inline bool delimiter(const char *string, const char *delimiters)
{
    register const char *j = delimiters;
    while (*j)
    {
        if (*j++ == *string)
        {
            return true;
        }
    }
    return false;
}

static void token_add(TOKENIZER *self, char *token)
{
    if (self->allow_escape_characters)
    {
        remove_escape_character(token, self->escape_character);
    }
    if (self->excludes)
    {
        size_t strings_to_exclude = stringlist_size(self->excludes);
        for (size_t i = 0; i < strings_to_exclude; ++i)
        {
            remove_string(token, stringlist_string(self->excludes, i));
        }
    }
    stringlist_add(self->tokens, token);
}

static void tokenize(TOKENIZER *self)
{
    register char *i = self->copy;
    bool currently_inside_token = false;
    bool previous_char_escape = false;
    char *token = NULL;
    bool is_delimiter;
    while (*i)
    {
        is_delimiter = delimiter(i, self->delimiters);
        if (is_delimiter && previous_char_escape)
        {
            is_delimiter = false;
        }
        if (!currently_inside_token && !is_delimiter)
        {
            /* Entering a token */
            token = i;
        }
        else if (is_delimiter)
        {
            if (currently_inside_token || self->allow_empty_tokens)
            {
                /* Exiting a token */
                *i = '\0';
                token_add(self, token);
                token = NULL;
            }
        }
        previous_char_escape = (*i == self->escape_character
                                && !previous_char_escape);
        currently_inside_token = !is_delimiter;
        ++i;
    }
    if (token || self->allow_empty_tokens)
    {
        token_add(self, token);
    }
}

STRINGLIST *tokenizer_create_tokens(TOKENIZER *self, const char *original)
{
    self->original = original;
    self->copy = Strdup(original);
    remove_line_ending(self->copy);
    self->strsep_ptr = self->copy;
    self->current_token = NULL;
    self->tokens = stringlist_new();
    tokenize(self);
    return self->tokens;
}

void tokenizer_free_tokens(TOKENIZER *self)
{
    stringlist_delete(self->tokens);
    Free(self->copy);
}
