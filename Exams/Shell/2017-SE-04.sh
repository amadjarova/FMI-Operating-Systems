#!/bin/bash

if [[ "${#}"  -ne 1 && "${#}" -ne 2 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -d "${1}" ]] ; then
        echo "First argument must be a directory" >&2
        exit 1
fi

if [[ "${#}" -eq 2 && ! -f "${2}" ]] ; then
        echo "Second argument must be a file" >&2
        exit 2
fi

counter=0
while read file ; do
        link=$(readlink "$file")
        if [[ "$#" -eq 2 ]] ; then
                if [[ ! -e "$file" ]] ; then
                        counter=$(("$counter" + 1))
                else
                        echo "$file -> $link" >>"$2"
                fi
        else
                if [[ ! -e "$file" ]] ; then
                        counter=$(("$counter" + 1))
                else
                        echo "$file -> $link"
                fi
        fi
done < <(find "${1}" -type l)

if [[ "$#" -eq 1 ]] ;then
        echo "Broken symlinks: ${counter}"
else
        echo "Broken symlinks: ${counter}" >> "$2"
fi
