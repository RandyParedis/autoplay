#!/usr/bin/env bash

am=0
IFS=$'\n'; set -f
for f in $(find . -name '*.mid'); do
    base=$(basename $f)
    base=$(echo $base | cut -d"." -f1)
    dir=$(dirname $f)
    dir=$(dirname ${dir:2})
    if [ ! -d "musicxml/$dir" ]; then
        mkdir -p musicxml/$dir
    fi
    (exec musescore -o musicxml/$dir/$base.xml $f &> /dev/null &)
    echo "$am)  $base.mid --> $base.xml"
    am=$((am+1))
done
unset IFS; set +f
echo "$am MIDI files found!"
