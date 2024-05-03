#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Invalid arguments count" >&2
        exit 1
fi

if [[ ! -d  "$1" || ! -d "$2" ]] ; then
        echo "Args 1 and 2 should be directories" >&2
        exit 2
fi

if [[ -n $(find "$2" -mindepth 1 2>/dev/null) ]] ; then
        echo "Directory: $2 must be empty" >&2
        exit 4
fi

while read file ; do
        name=$(basename "$file")

        if  echo "$name" | grep -q -E "^.*\.swp$" ; then
                originalName=$(echo "$name" | sed -E "s/^(.*)\.swp$/\1/")

                if [[  $(find $1 -regex ".*/$originalName$" | wc -l) -ne 0 ]] ; then
                  continue;
                fi

        fi

        newDir=$(echo "$file" | sed -E "s|$1(.*)|$2\1|")
        dir=$(echo "$newDir" |sed -E "s|(.*)/.*|\1|")
        mkdir -p "$dir"
        mv "$file" "$newDir"

done< <(find "$1" -type f)
