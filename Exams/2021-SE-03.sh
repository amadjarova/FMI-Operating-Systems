#!/bin/bash

if [[ "$#" -ne 2 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] ; then
        echo "Arg 1 should be a file" >&2
        exit 2
fi

touch "$2"

file1Size=$(stat -c '%s' "$1")

if [[ $(($file1Size % 2 )) -ne 0 ]] || [[ "$file1Size" -gt 524288 ]] ; then
        echo "File $1 : invalid size" >&2
        exit 3
fi

sizeOfArray=$(($file1Size / 2))
echo "#include <stdint.h>" >>"$2"
echo "uint32_t arrN = $sizeOfArray; " >> "$2"
echo -n "uint32_t arr[] = {" >>"$2"

index=0
for num in $(xxd "$1" | sed -E "s/^.*: *(.*)  .*$/\1/") ; do
        numForArr=$(echo "$num" | sed -E "s/(..)(..)/0x\2\1/")
        if [[ $index -ne 0 ]] ; then
                echo -n ',' >>"$2"
        fi
        echo -n "$numForArr" >>$2
        index=$(($index + 1))
done

echo "};" >>"$2"
