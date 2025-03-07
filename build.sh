#!/usr/bin/env bash

clang $1 -Wall -O3 -o $2
if [ $? -eq 0 ]; then
  echo "$0 ---> BUILD!"
  ./$2
else
  echo "$0 ---> FAIL!"
fi
