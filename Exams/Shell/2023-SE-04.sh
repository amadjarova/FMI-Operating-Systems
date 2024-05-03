#!/bin/bash

if [[ "$#" -ne 1 ]] ; then
        echo "Wrong args count" >&2
        exit 1
fi

if [[ ! -d "$1" ]] ; then
        echo "Param 1 must be a directory" >&2
        exit 2
fi

count=0
groups=$(mktemp)
index=0

while read file1 ; do
        index=$(( $index + 1))
        while read file2 ; do
                if [[ "$file1" != "$file2" ]] &&  [[ "$(sha256sum "$file1" | cut -d ' ' -f 1)" == "$(sha256sum "$file2" | cut -d ' ' -f 1)" ]] && [[ $(stat -c '%i' "$file1") -ne $(stat -c '%i' "$file2") ]]; then
                        count=$(( $count + $(stat -c '%s' "$file2" )))
                        rm "$file2"
                        ln "$file1" "$file2"
                        echo "$index" >> "$groups"
                fi
        done< <(find "$1" -type f)
done< <(find "$1" -type f)

groupsCount=$(cat "$groups" | sort | uniq | wc -l)
echo "Groups: $groupsCount, bytes: $count"

rm "$groups"
