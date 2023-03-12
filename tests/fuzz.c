#include "tests.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "missing argument\n");
        exit(1);
    }
    int len = atoi(argv[1]);
    char *data = malloc(len+1);
    assert(data);
    memset(data, 0, len+1);
    size_t n = fread(data, 1, len, stdin);
    assert(n == len);
    fuzztest(data, len);
    free(data);
    return 0;
}
