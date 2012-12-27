fieldx -- a field extraction utility

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

    $ echo A B C D | ./src/fieldx 2-3
    B C
    $ echo A B C D | ./src/fieldx 2-
    B C D
    $ echo A B C D | ./src/fieldx -- -3  # careful, looks like a flag
    A B C
    $ echo A B C D | ./src/fieldx -
    A B C D
