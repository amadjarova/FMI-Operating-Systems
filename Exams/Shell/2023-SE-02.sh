#!/bin/bash

if [[ "$#" -ne 3 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

counter=0
times=$1
shift
endTime=""
startTime=$(date +'%s.%N')

while [[ $(echo "$(date +'%s.%N')<$(echo "${startTime}+${times}"| bc -l)" |bc -l) -eq 1 ]] ; do
        "${@}"
        endTime=$(date +'%s.%N')
        counter=$(("$counter"+1))
done

echo "Ran the command ${*} ${counter} times for $(echo "${endTime}-${startTime}" | bc -l )  seconds"
