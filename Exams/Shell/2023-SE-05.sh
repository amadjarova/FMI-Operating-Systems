#!/bin/bash

found=true
commands=$(mktemp)
count=0

while $found; do
        found=false
        count=$(( $count + 1 ))
        for command in $(ps -e -o comm= | sort | uniq) ; do
                rss1=$(ps -e -o comm=,rss= | grep -E "^${command}\>" | tr -s ' '| cut -d ' ' -f 2 | awk '{sum+=$1} END {print sum}')
                if [[ $rss1 -gt 65536 ]] ; then
                        echo "$command" >> "$commands"
                        sleep 1
                        found=true
                fi
        done
done

half=$(($count / 2 ))
cat "$commands" | sort | uniq -c | tr -s ' ' | awk -v FS=" " -v h=$half '$1 >= h {print $2}'
rm $commands
