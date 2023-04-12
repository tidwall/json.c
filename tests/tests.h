#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <assert.h>
#include "../json.h"

static bool nameless_tests = false;

#define do_test(name) { \
    if (argc < 2 || strstr(#name, argv[1])) { \
        if (!nameless_tests) printf("%s\n", #name); \
        name(); \
    } \
}

double now(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec*1e9 + now.tv_nsec) / 1e9;
}

char *readfile(const char *path, long *len) {
    FILE *f = fopen(path, "rb+");
    assert(f);
    assert(fseek(f, 0, 2) == 0);
    size_t size = ftell(f);
    rewind(f);
    char *mem = (char *)malloc(size+1);
    assert(mem);
    assert(fread(mem, size, 1, f) == 1);
    mem[size] = '\0';
    assert(fclose(f) == 0);
    if (len) *len = size;
    return (char*)mem;
}

char *readtestfile(const char *name, long *len) {
    char path[128];
    snprintf(path, sizeof(path), "testfiles/%s", name);
    return readfile(path, len);
}

char *commaize(unsigned int n) {
    char s1[64];
    char *s2 = (char *)malloc(64);
    assert(s2);
    memset(s2, 0, sizeof(64));
    snprintf(s1, sizeof(s1), "%d", n);
    int i = strlen(s1)-1; 
    int j = 0;
    while (i >= 0) {
        if (j%3 == 0 && j != 0) {
            memmove(s2+1, s2, strlen(s2)+1);
            s2[0] = ',';
        }
        memmove(s2+1, s2, strlen(s2)+1);
        s2[0] = s1[i];
        i--;
        j++;
    }
    return s2;
}

void touch_all(struct json json) {
    while (1) {
        json = json_ensure(json);
        if (!json_exists(json)) break;
        if (json_type(json) == JSON_STRING) {
            long len = json_string_length(json);
            char *mem = malloc(len+1);
            assert(mem);
            long len2 = json_string_copy(json, mem, len+1);
            assert(len == len2);
            // printf("%.*s\n", (int)len, mem);
            free(mem);
        }
        touch_all(json_first(json));
        json = json_next(json);
    }
}

void fuzztest(char *json, int len) {
    json_validn(json, len);
    touch_all(json_parsen(json, len));
} 

char *names[] = {
    "apache_builds.json",
    "canada.json",
    "citm_catalog.json",
    "github_events.json",
    "gsoc-2018.json",
    "instruments.json",
    "marine_ik.json",
    "mesh.json",
    "mesh.pretty.json",
    "numbers.json",
    "random.json",
    "twitter.json",
    "twitterescaped.json",
    "update-center.json",
    
};




#endif // TESTS_H
