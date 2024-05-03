#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] ; then
        echo "First argument must be a file" >&2
        exit 2
fi

if [[ ! -d "$2" ]] ; then
        echo "Second argument must be a directory" >&2
        exit 3
fi

if [[ -n $(find "$2" -mindepth 1 2>/dev/null) ]] ; then
        echo "The directory must be empty" >&2
        exit 4
fi

fileName="$2"/dict.txt
touch "$fileName"
counter=1

while read line ; do
        name=$(echo "$line" | sed -E 's/^([A-Za-z-]+) *([A-Za-z-]+) *(\(.*\))* *:.*$/\1 \2/g')
        if [[ $(grep -E "^$name" "$fileName" | wc -l) -eq 0 ]] ; then
                echo "$name;$counter" >>"$fileName"
                counter=$(("$counter" + 1))
        fi
done < "${1}"

while read line ; do
        num=$(echo "$line" | cut -d ';' -f2 )
        actName=$(echo "$line" | cut -d ';' -f1 )
        firstName=$(echo "$actName" | cut -d ' ' -f1)
        secondName=$(echo "$actName" | cut -d ' ' -f2)
        file2Name="${2}/${num}.txt"
        touch "${file2Name}"
        grep -E "^${firstName} *${secondName}" "$1" 1>>"$file2Name"
done < "$fileName"
