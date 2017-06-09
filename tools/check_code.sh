#!/usr/bin/env sh

cppcheck -q --force --enable=all --inconclusive --suppress=missingIncludeSystem -I ./include ./
