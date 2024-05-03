#!/bin/bash

if [[ "$#" -ne 3 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] ; then
        echo "First argument must be a file" >&2
        exit 2
fi

if !  grep -q -E "^$2=" "$1" ; then
        echo "First string not found " >&2
        exit 3
fi

if grep -q -E "^$3=" "$1" ; then
        firstValues=$(grep -E "^$2=" "$1" | cut -d '=' -f 2 | tr -s " ")
        secondValues=$(grep -E "^$3=" "$1" | cut -d '=' -f 2 | tr -s " ")
        
        for value1 in $firstValues ; do
                for value2 in $secondValues ; do
                        if [[ "$value1" == "$value2" ]] ; then
                                sed -i -E "s/(^$3=.*)$value2 *(.*)$/\1\2/" "$1"
                        fi
                done
        done
fi
