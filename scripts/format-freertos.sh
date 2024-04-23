find sdk/freertos_app_cpu0/ -iname *.h -o -iname *.c | xargs ./scripts/clang-format -style=file -i
find sdk/freertos_app_cpu1/ -iname *.h -o -iname *.c | xargs ./scripts/clang-format -style=file -i
