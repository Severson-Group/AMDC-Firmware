#!/usr/bin/env bash

result=0
find sdk/bare/ -iname *.h -o -iname *.c | xargs ./scripts/clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null
if [ $? -ne 1 ]; then 
    echo "Formatting mismatch, please run clang-format"
    result=1
else
    echo "Formatting matches!"
fi

exit $result
