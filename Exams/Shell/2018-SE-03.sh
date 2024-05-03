#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Wrong arguments count." >&2
        exit 2
fi

if [[ ! -f "$1" || ! -f "$2" ]] ; then
        echo "$1 and $2 must be files" >&2
        exit 2
fi

while read row ; do
        num=$(echo "$row" | cut -d ',' -f 1 )
        withoutNum=$(echo "$row" | sed -E 's/[0-9]+,(.*)/\1/')

        if grep -q -E "^[0-9]+,$withoutNum" "$2" ; then
                match=$(grep -E "^[0-9]+,$withoutNum$" "$2")
                matchNum=$(echo "$match" | cut -d ',' -f 1)

                if [[ "$num" -gt "$matchNum" ]] ; then
                        sed -i -E "s/$match/$row/g" "$2"
                fi
        else
                echo "$row" >> "$2"
        fi
done < "$1"
