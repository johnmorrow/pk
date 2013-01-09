# fieldx -- a field extraction utility

Do you use lots of shell pipelines and find yourself choosing between _cut_
and _awk_ to select columns from input? _fieldx_ is designed as a middle-ground
tool; flexible enough to handle variable numbers of delimiters, fixed format
files, quoted or escaped delimiters and more.

Usage
-----

```shell
Usage: fieldx [OPTION...] [STRING...]
A field extraction utility

  -b, --backslash            Backslash escapes delimiters
  -d, --delimiters=STRING    Characters used as input delimiters
  -e, --empty                Allow empty fields
  -E, --excludes[=STRINGS]   Strings excluded from output (separated by :)
  -f, --file=FILE            Read input from file instead of stdin
  -N, --null[=STRING]        Change output text used for empty fields
  -q, --quotes[=STRING]      Ignore delimiters within quotes
  -S, --separator=STRING     Separator used in output text
  -T, --trim                 Trim non-alphanumerics characters before printing
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```

It may help to remember that when using short flags, lower-case flags
will affect how the input is processed, for example, changing the field
delimiters. Upper-case flags will affect the output of fieldx, for example,
the output separator or whether tokens are trimmed of alphanumerics.

Examples
--------

#### Basic usage

_fieldx_ is a tool for grabbing columns from an input stream
and printing them on _stdout_. Each argument represents something
you would like to see appearing in the output.

If an argument string is a positive integer then it is a request
to see that column from the input printed in the output. The first
column in the input stream has an index of 1.

```shell
$ df | fieldx 6 5
Mounted Use%
/ 9%
/dev 1%
/run 1%
/run/lock 0%
/run/shm 0%
```

An argument string can also represent a range of fields.
The following range formats are supported:

| Format | Meaning                                                 |
|--------|---------------------------------------------------------|
| X-Y    | Print each field from index X through to index Y        |
| X-     | Print each field from index X through to the last field |
| -Y     | Print each field from field 1 through to field Y        |
| -      | Print every field                                       |

Here are examples of each type of range:

```shell
$ echo A B C D E | fieldx 2-4
B C D
$ echo A B C D E | fieldx 2-
B C D E
$ echo A B C D E | fieldx -- -3  # careful, looks like a flag
A B C
$ echo A B C D E | fieldx -
A B C D E
```

Finally, any argument that does not appear to be an index or
a range is outputted directly. This can be useful for putting
together command lines or quick scripts:

```shell
$ ls -l /etc | fieldx 9 is owned by 3
adduser.conf is owned by root
alternatives/ is owned by root
apparmor/ is owned by root
```

#### Changing the delimiters

In its default mode of operation _fieldx_ uses both _<tab>_ and
_<space>_ as delimiter characters. Additionally, multiple delimiters
appearing next to each other in the input stream are treated as
a single delimiter. This makes it good for parsing input streams
where the columns are separated by varying amounts of whitespace.
Many system tools such as _ps_, _df_, _last_, etc. use this kind
of output.

By using the _-d_ flag you can change the set of delimiters. Note
that the string argument to _-d_ contains a set of characters, each
of which is a delimiter. This is a not a fixed string that separates
each field.

#### The output separator

By default the field specified for printed will be outputed separated
by single spaces. Use the _-S_ flag to change the string that separates
output fields:

```shell
$ ps aux | fieldx -S, 1 2 | head
USER,PID
root,1
root,2
root,3
```

#### Fixed format files

Fixed formats, such as */etc/passwd*, can have different delimiters and
possibly contain empty fields. Using the _-e_ flag tells _fieldx_ that
adjacent delimiter characters in the input line represent empty fields.

```shell
$ cat /etc/passwd | fieldx -e -d: 1 7
root /bin/bash
daemon /bin/sh
bin /bin/sh
sys /bin/sh
```

#### Empty fields

If you tell _fieldx_ to print an empty field it will print out the string
*NULL* by default. This can be set to another string using the _-N_ flag.
If this flag is used without an argument then empty fields are not printed.

Note that _fieldx_ will not print trailing empty fields unless you specify
them directly using their index. i.e. trailing empty fields are not printed
if specified as a range.

#### Trimming non-alphanumeric fields

The *-T* flags trims non-alphanumeric characters from the left and right side
before printing the field to stdout. This is useful removing quotes,
parentheses or other visual delineations.

```shell
$ echo "'Example User' <foobar@example.com>" | fieldx -T 3
foobar@example.com
```

#### Quoting

Some text formats allow delimiters to be ignored inside quotes.
_fieldx_ supports simple quoting, that is you can select a single
quote character or a pair (start and end) of characters. Nested
quotes or multiple types of quoting on the same line are not
supported.

Use the _-q_ flag to enable quoting support. Double quote is the
default quote character:

```shell
$ cat input
"Bilbo Baggins", "The Hobbit"
"Frodo Baggins", "The Lord of the Rings"
$ fieldx -f input -d", " -q 2
"The Hobbit"
"The Lord of the Rings"
```

The quote character can be changed by supplying a one character
argument to the _-q_ flag:

```shell
$ cat input
'Bilbo Baggins', 'The Hobbit'
'Frodo Baggins', 'The Lord of the Rings'
$ fieldx -f input -d", " -q"'" 1
'Bilbo Baggins'
'Frodo Baggins'
```

A two character argument supplied to the _-q_ flag is used to
specify the open and close quote characters:

```shell
$ cat input
(Bilbo Baggins) (The Hobbit)
(Frodo Baggins) (The Lord of the Rings)
$ fieldx -f input -q"()" 1
(Bilbo Baggins)
(Frodo Baggins)
$ fieldx -f input -q"()" -T 1
Bilbo Baggins
Frodo Baggins
```

#### Excludes

A list of strings that will always be excluded from the output can be supplied
to fieldx via the *-E* flag. An example use case for this is when dealing with
lists of servers you may want to automatically strip fully qualified hostnames
down to their local names before passing on to another process in the pipeline.

```shell
$ cat input
foo.example.com 192.168.1.1 active
bar.example.com 192.168.1.2 repair
baz.example.net 192.168.1.3 active
$ cat input | fieldx -E.example.com:.example.net 1 3
foo active
bar repair
baz active
```

Alternatively, as this is likely to be a regular request, it can be set using an
environment variable. When using the environment variable the *-E* flag without
arguments can be used to ignore the setting. The *-E* flag used with arguments
will override the environment variable.

```shell
$ export FIELDX_EXCLUDES=.example.com:.example.net
$ cat input | fieldx 1 3
foo active
bar repair
baz active
$ cat input | fieldx -E 1 3
foo.example.com active
bar.example.com repair
baz.example.net active
```

Licensing
---------

Copyright 2012-Present John Morrow

I am providing code in this repository to you under an open source license.
Because this is my personal repository, the license you receive to my code
is from me and not my employer (Facebook).

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
