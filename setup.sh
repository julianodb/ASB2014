#! /bin/bash

# file: setup.sh

# This bash script performs any setup necessary in order to test your entry.
# It is run only once, before running any other code belonging to your entry.

# In this case, the code for the entry is a C program called 'gqrs.c', within
# the 'sources' subdirectory.  The command below compiles the C program into an
# executable binary named 'gqrs', linking it with the WFDB library.

cc -o gqrs -O sources/gqrs.c -lwfdb

# In your entry, you should replace the command above with one or more commands
# that compile your code if necessary, so that it can be run by 'next.sh'.  You
# may assume that the WFDB library is available and that it can be linked as in
# the example above.  Other standard libraries can be used similarly.  If your
# code requires any custom or non-standard libraries, include their sources in
# your entry, together with commands in this file to compile them.

# If your code is written in an interpreted language, it may not be necessary to
# perform any setup;  in this case, simply delete the command above.  Note,
# however, that 'setup.sh' must be included in your entry in any case;  if it
# is missing, or if running it causes an error, your entry will not be scored.
