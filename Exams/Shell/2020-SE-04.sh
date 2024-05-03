#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Wrong arguments count" >&2
        exit 3
fi

if [[ ! -d "$1" || ! -d "$2" ]] ; then
        echo "$1 and $2 must be directories" >&2
        exit 2
fi

while read file ; do
        name=$(basename "$file")
        title=$(echo "$name" | sed -E "s/\([^\)]*\)//g" |sed -E "s/(.*) *\.jpg/\1/"| tr -s ' ')
        album="misc"

        if echo "$name" | grep -E ".*\(.*\).*" ; then
                album=$(echo "$name" | sed -E "s/.*\((.*)\).*.jpg/\1/")
        fi

        date=$(date -d "$(stat -c "%y" "$file")" +'%Y-%m-%d')
        hashh=$(sha256sum "$file"| head -c 16)

        fileCopy="$2"/images/"$hashh".jpg
        fc2=images/"$hashh".jpg
        mkdir -p "$(dirname "$fileCopy")"
        cp -r "$file" "$fileCopy"

        f="$2"/by-date/"$date"/by-album/"$album"/by-title/"$title".jpg
        mkdir -p "$(dirname "$f")"
        ln -s ../../../../../"$fc2" "$f"

        f="$2"/by-date/"$date"/by-title/"$title".jpg
        mkdir -p "$(dirname "$f")"
        ln -s ../../../"$fc2" "$f"

        f="$2"/by-album/"$album"/by-date/"$date"/by-title/"$title".jpg
        mkdir -p "$(dirname "$f")"
        ln -s ../../../../../"$fc2" "$f"

        f="$2"/by-title/"$title".jpg
        mkdir -p "$(dirname "$f")"
        ln -s ../"$fc2" "$f"

done< <(find "$1" -type f -name "*.jpg")
