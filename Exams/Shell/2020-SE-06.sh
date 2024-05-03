#!/bin/bash

if [[ "$#" -ne 3 ]] ; then
        echo "Wrong args count" >&2
        exit 2
fi

if [[ ! -f "$1" ]] ; then
        echo "First arg should be a file" >&2
        exit 1
fi

while read row ; do
        if ! echo "$row" | grep -q -E "^ *[A-Za-z0-9_]+ *= *[A-Za-z0-9_]+ *(#.*)*$" ; then
                if ! echo "$row" | grep -q -E "^ *$" ; then
                        if ! echo "$row" | grep -q -E "^ *#.*$" ; then
                          echo "Invalid file format" >&2
                          exit 3
                        fi
                fi
        fi
done < "$1"

if grep -q -E "^ *$2 *=" "$1" ;then
        sed -i -E "s/^ *$2 *=.*/#\0 # edited at $(date) by $(whoami)\n $2 = $3 #added at $(date) by $(whoami)/" "$1"
else
        echo "$2 = $3 # added at $(date) by $(whoami)" >> "$1"
fi
