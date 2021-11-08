#!/usr/bin/env bash

result=0
find sdk/app_cpu0 -iname *.h -o -iname *.c | xargs ./scripts/clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null

if [ $? -ne 1 ]; then 
    echo "Formatting mismatch, please run clang-format"
    result=1

    find sdk/app_cpu0/ -iname *.h -o -iname *.c | xargs python ./scripts/run-clang-format.py --clang-format-executable=./scripts/clang-format
else
    echo "Formatting matches!"
fi

exit $result
