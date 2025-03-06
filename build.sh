#!/usr/bin/env bash

clang main.c -Wall -O3 -o main
if [ $? -eq 0 ]; then
  echo BUILDED!
else
  echo FAIL!
fi
