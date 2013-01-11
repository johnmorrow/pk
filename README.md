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

Select and rearrange output from column based commands:

```shell
$ df | fieldx 6 5
Mounted Use%
/ 9%
/dev 1%
/run 1%
/run/lock 0%
/run/shm 0%
```

Non-numeric fields are outputted directly:

```shell
$ ls -l /etc | fieldx 9 is owned by 3
adduser.conf is owned by root
alternatives/ is owned by root
apparmor/ is owned by root
```

Ranges are also supported:

```shell
$ echo A B C D | fieldx 2-3
B C
$ echo A B C D | fieldx 2-
B C D
$ echo A B C D | fieldx -- -3  # careful, looks like a flag
A B C
$ echo A B C D | fieldx -
A B C D
```

Change the output separator to create new file formats:

```shell
$ ps aux | fieldx -S, 1 2 | head
USER,PID
root,1
root,2
root,3
```

#### Fixed format files

Fixed formats, such as */etc/passwd*, can have different delimiters and
possibly contain empty fields:

```shell
$ cat /etc/passwd | fieldx -e -d: 1 7
root /bin/bash
daemon /bin/sh
bin /bin/sh
sys /bin/sh
```

#### Trimming non-alphanumeric fields

The *-T* flags trims non-alphanumeric characters from the left and right side
before printing the field to stdout. This is useful removing quotes,
parentheses or other visual delineations.

```shell
$ echo "'Example User' <foobar@example.com>" | fieldx -T 3
foobar@example.com
```

#### Quoting

Quoting is supported. Double quote is the default quote character:

```shell
$ cat input
"Bilbo Baggins", "The Hobbit"
"Frodo Baggins", "The Lord of the Rings"
$ fieldx -f input -d", " -q 2
"The Hobbit"
"The Lord of the Rings"
```

The quote character can be specified. 

```shell
$ cat input
'Bilbo Baggins', 'The Hobbit'
'Frodo Baggins', 'The Lord of the Rings'
$ fieldx -f input -d", " -q"'" 1
'Bilbo Baggins'
'Frodo Baggins'
```

A two character argument supplied to the quote flag are used as start and end:

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
