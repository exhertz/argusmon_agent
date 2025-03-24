#!/usr/bin/env bash

clang $1 -Wall -O3 -o $2
if [ $? -eq 0 ]; then
  echo "$0 ---> BUILD!"
  echo "$0 ---> $2 running:"
  ./$2
else
  echo "$0 ---> FAIL!"
fi
