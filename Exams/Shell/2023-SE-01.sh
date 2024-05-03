#!/bin/bash

if [[ $# -ne 2 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] ; then
        echo "Argument 1 should be a file" >&2
        exit 2
fi

if [[ ! -d "$2" ]] ; then
        echo "Argument 2 should be a directory" >&2
        exit 3
fi

while read file; do
        while read word; do
                replace="$(echo "${word}" | tr '[a-zA-Z0-9_]' '*')"
                sed -E -i "s/\<${word}\>/${replace}/gi" "${file}"
        done < "${1}"
done < <(find "${2}" -type f -name '*.txt' 2>/dev/null)
