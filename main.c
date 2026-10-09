#include "options.h"
#include "list.h"

#include <stdio.h>

int main(int argc, char **argv) {
    Options opts;

    options_init(&opts);

    int first_operand = options_parse(&opts, argc, argv);
    if (first_operand < 0) {
        return 1;
    }

    list_main(&opts, argc - first_operand, argv + first_operand);

    return 0;
}
