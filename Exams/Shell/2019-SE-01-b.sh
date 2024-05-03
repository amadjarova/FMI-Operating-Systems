#!/bin/bash

maxSum=0
minNum=10000000

while read row ; do
        if [[ "$row" =~ ^(-){,1}[0-9]+$ ]] ; then
                num=$(echo "$row" | sed -E "s/^-*([0-9]+)$/\1/" )
                sum=0

                while read row2 ; do
                        sum=$((sum + "$row2"))
                done < <(echo "$num" | sed -E "s/./\0\n/g" | head -n -1)

                if [[ "$sum" -gt "$maxSum" ]] ; then
                        maxSum="$sum"
                        minNum="$row"
                elif [[ "$sum" -eq "$maxSum" ]] ; then
                        if [[ "$row" -lt "$minNum" ]] ; then
                                minNum="$row"
                        fi
                fi
        fi
done
echo "$minNum"
