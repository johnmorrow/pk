fieldx -- a field extraction utility

#### Usage

    Usage: fieldx [OPTION...] [STRING...]
    A field extraction utility

      -b, --backslash            Backslash escapes delimiters
      -d, --delimiters=STRING    Characters used as input delimiters
      -e, --empty                Allow empty fields
      -E, --excludes[=STRINGS]   Strings excluded from output (separated by :)
      -f, --file=FILE            Read input from file instead of stdin
      -n, --null[=STRING]        Change output text used for empty fields
      -q, --quotes[=STRING]      Ignore delimiters within quotes
      -s, --separator=STRING     Separator used in output text
      -t, --trim                 Trim non-alphanumerics characters before printing
      -?, --help                 Give this help list
          --usage                Give a short usage message
      -V, --version              Print program version

#### Examples

Select and rearrage output from column based commands:

    $ df | fieldx 6 5
    Mounted Use%
    / 9%
    /dev 1%
    /run 1%
    /run/lock 0%
    /run/shm 0%

Non-numeric fields are outputted directly:

    $ ls -l /etc | fieldx 9 is owned by 3
    adduser.conf is owned by root
    alternatives/ is owned by root
    apparmor/ is owned by root

Fixed formats, such as /etc/passwd, can have different delimiters and
possibly contain empty fields:

    $ cat /etc/passwd | fieldx -e -d: 1 7
    root /bin/bash
    daemon /bin/sh
    bin /bin/sh
    sys /bin/sh

Change the output separator to create new file formats:

    $ ps aux | fieldx -s, 1 2 | head
    USER,PID
    root,1
    root,2
    root,3

Ranges are also supported:

    $ echo A B C D | fieldx 2-3
    B C
    $ echo A B C D | fieldx 2-
    B C D
    $ echo A B C D | fieldx -- -3  # careful, looks like a flag
    A B C
    $ echo A B C D | fieldx -
    A B C D

The -t flags trims non-alphanumeric characters from the left and right side
before printing the field to stdout. This is useful removing quotes,
parentheses or other visual delineations.

    $ echo "'Example User' <foobar@example.com>" | fieldx -t 3
    foobar@example.com

Quoting is supported. Double quote is the default quote character:

    $ cat input
    "Bilbo Baggins", "The Hobbit"
    "Frodo Baggins", "The Lord of the Rings"
    $ fieldx -f input -d", " -q 2
    "The Hobbit"
    "The Lord of the Rings"

The quote character can be specified. 

    $ cat input
    'Bilbo Baggins', 'The Hobbit'
    'Frodo Baggins', 'The Lord of the Rings'
    $ fieldx -f input -d", " -q"'" 1
    'Bilbo Baggins'
    'Frodo Baggins'

A two character argument supplied to the quote flag are used as start and end:

    $ cat input
    (Bilbo Baggins) (The Hobbit)
    (Frodo Baggins) (The Lord of the Rings)
    $ fieldx -f input -d", " -q"()" 1
    (Bilbo Baggins)
    (Frodo Baggins)
    $ fieldx -f input -d", " -q"()" -t 1
    Bilbo Baggins
    Frodo Baggins


Disclaimer
----------

I am providing code in this repository to you under an open source license.
Because this is my personal repository, the license you receive to my code
is from me and not my employer (Facebook).
