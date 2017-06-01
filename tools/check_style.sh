#!/usr/bin/env sh

FILES=$(find ./ -iname *.h -o -iname *.c -o -iname *.cc -o -iname *.hpp -o -iname *.cpp)

if [ "$1" = "--fix" ]
then
  for FILE in $FILES; do clang-format-3.6 -style=file -i $FILE; done
else
  for FILE in $FILES; do clang-format-3.6 -style=file $FILE | diff $FILE -; done
fi
