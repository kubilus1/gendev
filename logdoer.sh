#!/bin/bash

while read line; do
    printf '\r%s %s' "$(tput el)" "$line"
    sleep 0.25
done < "/dev/stdin"
