#!/bin/bash

if [[ "$#" -ne 1 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ ! -f "$1" ]] ; then
        echo "Argument 1 must be a file" >&2
        exit 2
fi

result=$(mktemp)

while read row ; do
        client=$(echo "$row" | cut -d ' ' -f 2)
        adr=$(echo "$row" | cut -d ' ' -f 1)
        version=no
        if echo "$row"| grep -q -E "^.*HTTP/2\.0.*" ;then
                version=yes
        fi
        code=$(echo "$row" | sed -E "s|^.*HTTP/[0-9]\.[0-9] ([0-9]+).*$|\1|")
        echo "$client $version $code $adr">>"$result"
done< "$1"

while read cl ; do
        http2=$(grep -E "^$cl yes.*$" "$result" | wc -l)
        http1=$(grep -E "^$cl no.*$" "$result" | wc -l)
        echo "$cl HTTP/2.0: $http2 non-HTTP/2.0: $http1"

        result2=$(mktemp)

        while read row ; do
                if [[ $(echo "$row" | cut -d ' ' -f3) -gt 302 ]] ; then
                        echo "$(echo $row | cut -d ' ' -f 4)" >> "$result2"
                fi
        done< <(cat "$result" | grep -E "^$cl .*")

        if [[ $(cat "$result2" | wc -l) -ge 1 ]] ; then
        echo "$(cat $result2 | sort | uniq -c | tr -s ' ' | sort -t ' ' -k 1 -r | head -n 5)"
        fi

        rm "$result2"
done< <(cat "$result" |cut -d ' ' -f 1| sort | uniq -c |tr -s ' '|sort -n -t ' ' -k 2 | cut -d ' ' -f 3| head -n 3)

rm "$result"
