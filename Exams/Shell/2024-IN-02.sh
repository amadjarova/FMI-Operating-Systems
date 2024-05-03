#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Invalid args count" >&2
        exit 2
fi

if [[ ! -d "$1" ]] ; then
        echo "Arg 1 should be a directory" >&2
        exit 3
fi

if [[ ! -f "$3" ]] ; then
        echo "Arg 2 should be a file" >&2
        exit 4
fi

while read file ; do
 while read row; do
        className=$(echo "$row" |sed -E "s/^ *class ([a-zA-Z0-9_]+).*$/\1/")
        if ! grep -q -E "^$className$" "$2" ; then
                echo "$className" >> "$2"
        fi

        if echo "$row" | grep -q -E "^.*:.*$"; then
        while read class ; do
                className1=$(echo "$class" | sed -E "s/^ *(private|public|protected)* *([A-Za-z0-9_]+).*$/\2/")
                echo "$className1"
                
                if ! grep -q -E "^$className1$" "$2" ; then
                        echo "$className1" >> "$2"
                fi
                
                echo "$className1 -> $className" >> "$2"
        done< <(echo "$row" | sed -E "s/^.*:(.*$)/\1/g" | tr ',' '\n')
        fi
 done < "$file"
done< <(find "$1" -name "*.h" 2>/dev/null)
