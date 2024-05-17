#!/bin/bash

if [[ "$#" -ne 1 ]] ; then
        echo "Wrong arguments count" >&2
        exit 1
fi

if [[ !  "$1" =~ [0-9]+ ]] ; then
        echo "Arg 1 must be a number" >&2
        exit 6
fi

if [[ $(whoami) != "oracle" ]] && [[ $(whoami) != "grid" ]] ; then
        echo "Invalid user" >&2
        exit 3
fi

if [[ ! -d "/u01/app/$(whoami)" ]] ; then
        echo "Dir not found" >&2
        exit 2
fi

if [[ -z "$ORACLE_HOME" ]] ; then
        echo "ORACLE_HOME not set" >&2
        exit 4
fi

if ! echo "$ORACLE_HOME" | grep -qE "/.+/.+/.+" ; then
        echo "Invalid ORACLE_HOME" >&2
        exit 5
fi

if [[ ! -x "$ORACLE_HOME/bin/adrci" ]] ; then
        echo "Executable file adrci not found" >&2
        exit 3
fi

while read home ; do
        "$ORACLE_HOME"/bin/adrci exec="SET BASE /u01/app/$(whoami); SET HOMEPATH /u01/app/$(whoami)/${home}; PURGE -AGE $1"
done < <("$ORACLE_HOME"/bin/adrci exec="SET BASE /u01/app/$(whoami); SHOW HOMES" | tail -n +2 | grep -E ".+/(crs|tnslsnr|kfod|asm|rdbms)/.*")
