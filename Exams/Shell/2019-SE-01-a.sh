#!/bin/bash

file=$(mktemp)
max=0

while read row ; do
        if [[ $row =~ ^(-){,1}[0-9]+$ ]] ; then
                num=$(echo "$row" | sed -E "s/^-*([0-9]+)$/\1/")

                if [[ $num -gt $max ]] ; then
                        max=$num
                        echo "$row" > "$file"
                elif [[ $num -eq $max ]] ; then
                        echo "$row" >> "$file"
                fi
        fi
done

cat "$file" | sort -n | uniq
rm "$file"
