#!/bin/bash

if [[ $# -ne 3 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] || [[ ! -f "$2" ]] ; then
        echo "Args 1 and 2 must be files" >&2
        exit 2
fi

if [[ ! -d "$3" ]] ; then
        echo "Arg 2 must be a directory" >&2
        exit 3
fi

if [[ ! -w "$2" ]] ; then
        echo "Write permission needed: $2" >&2
        exit 4
fi

passwordFile=$(mktemp)

while read file ; do
        foundError=false
        counter=1

        while read row ; do
                if ! echo "$row" | grep -qE "^$" && ! echo "$row" | grep -qE "^{ .* };$" ; then
                        if [[ "$foundError" == false ]] ; then
                                echo "Error in $file:"
                                foundError=true
                        fi

                        echo "Line $counter:$row"
                fi

                counter=$(($counter + 1))
        done< <(cat "$file" | cut -d '#' -f 1)

        if [[ "$foundError" == false ]] ; then
                cat "$file" >> "$2"
                name=$(basename $file)
                username=$(echo "$name" | sed -E "s/(.*)\.cfg/\1/")

                if ! cat "$1" | grep -qE "^$username:.*$" ; then
                        pwgen 16 >> "$passwordFile"
                        password=$(cat $passwordFile)
                        hashSum="$(md5sum $passwordFile | cut -d ' ' -f 1)"
                        echo "$username:$hashSum" >> "$1"
                        echo "$username $password"
                fi
        fi
done< <(find "$3" -mindepth 1 -name '*.cfg')

rm "$passwordFile"
