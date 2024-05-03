#!/bin/bash

if [[ "$#" -ne 3 ]] ; then
        echo "Wrong args count" >&2
        exit 1
fi

if [[ ! -f "$1" || ! -f "$2" ]] ; then
        echo "Args 1 and 2 must be files" >&2
        exit 2
fi

if ! grep -q -E "^$3:" "$1" || ! grep -q -E "^$3:" "$2" ; then
        echo "Arg 3 not found in the files" >&2
        exit 3
fi

while read row ; do
        if ! grep -q -E "^.*: [0-9]+ megaparsecs$" ; then
                echo "Invalid file format : $1" >&2
                exit 4
        fi
done < "$1"

while read row ; do
        if ! grep -q -E "^.*: [0-9]+ megaparsecs$" ; then
                echo "Invalid file format: $2" >&2
                exit 5
        fi
done < "$2"

first=$(grep -E "^$3=" "$1" | cut -d ':' -f 2 | cut -d ' ' -f 1)
second=$(grep -E "^$3=" "$2" | cut -d ':' -f 2 | cut -d ' ' -f 1)

if [[ $first -le "$second" ]] ; then
        echo "$(basename $1)"
else
        echo "$(basename $2)"
fi
