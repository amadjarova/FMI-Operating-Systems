#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Wrong args count" >&2
        exit 4
fi

if [[ ! -d "$1" ]] ; then
        echo "Dir not found" >&2
        exit 3
fi

mkdir -p "$2"

photos=$(mktemp)

find "$1" -type f -name '*.jpg' -exec stat {} -c '%y' \;  2>/dev/null > "$photos"

begin=''
end=''
while read cur ; do
        if [[ -z "$begin" ]] ; then
                begin="$cur"
                end="$begin"
        else
                if [[ $(date -d "$end + 1 day" +'%Y-%m-%d') == "$cur" ]] ; then
                        end="$cur"
                else
                        dirName="${begin}_${end}"
                        mkdir -p "$2"/"$dirName"
                        begin="$cur"
                        end="$begin"
                fi
        fi

done< <(cat "$photos" | cut -d ' ' -f 1 | sort | uniq)
dirName="${begin}_${end}"
mkdir -p "$2"/"$dirName"
rm "$photos"

while read file ; do
    photo=$(basename $file '.jpg')
    mod=$(stat "$file" -c '%y %Y')
    modDate=$(echo "$mod" | cut -d ' ' -f 1)
    modTime=$(echo "$mod" | cut -d ' ' -f 2 | cut -d '.' -f 1)
    modTimeStamp=$(echo $mod | cut -d ' ' -f 4)
    while read dir; do
                begin=$(date -d $(basename  $dir | cut -d '_' -f 1) +'%s' )
                end=$(date -d "$(basename $dir | cut -d '_' -f 2) + 1 day" +'%s')
                #end=$(($end - 1))
                if [[ $begin -le $modTimeStamp && $end -ge $modTimeStamp ]]; then
                        mv $file  "$dir/${modDate}_${modTime}.jpg"
                fi
    done < <(find $2 -mindepth 1 -type d 2>/dev/null)
done< <(find "$1" -type f -name '*.jpg' 2>/dev/null)
