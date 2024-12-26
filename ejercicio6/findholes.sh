#!/bin/sh

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <extension>"
    exit 1
fi

extension="$1"


for i in $(seq -w 0 999); do
    if [ ! -f "$i.$extension" ]; then
        echo "touch $i.$extension"
    fi
done
