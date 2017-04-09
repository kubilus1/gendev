#!/bin/bash

while read line; do
    printf '\r%s %s' "$(tput el)" "$line"
done < "/dev/stdin"
