#!/bin/bash

gcc -O3 -Wall -Wpedantic -Wextra -Werror mcdc.c ./test/strncmp.c -o test1.exe -I./ && \
gcc -g3 -ggdb mcdc.c ./test/strncmp.c -o test1.exe -I./
