#!/bin/bash

if [[ ! "$#" -eq 1 ]] ; then
        echo one argument needed >&2
        exit 1
fi

if [[ ! -d "$1" ]] ; then
        echo argument 1 must be a directory >&2
        exit 2
fi

count=0

while read file ; do
        if [[ ! -e "$file" ]] ; then
                count=$(("$count" + 1))
        fi
done < <(find "$1" -type l)

echo "$count"
