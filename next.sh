#! /bin/bash

# file: next.sh

# This bash script analyzes the record named in its command-line argument ($1),
# saving the results as an annotation file for the record, with annotator 'qrs'.
# This script is run once for each record in the Challenge test set.

# For example, if invoked as
#    next.sh 100
# it analyzes record 100 using 'gqrs', and saves the results as an annotation
# file named '100.qrs'.

./main -r $1

# In your entry, you should replace the command above with one or more commands
# that will produce a '.qrs' annotation file using your own code.

