#include "tests.h"

#define bench(name, nsecs, fn) \
    int count = 0; \
    double start = now(); \
    size_t isize = 0; \
    size_t tsize = 0; \
    while (now()-start < (nsecs)) { \
        for (int i=0;i<10;i++) { \
            isize = len; \
            fn; \
            tsize += isize; \
            count++; \
        } \
    } \
    double elapsed = now()-start; \
    char *bsize[] = { "MB", "GB", "TB" }; int bidx = 0; \
    double bsecs = ((double)tsize)/elapsed/1024/1024; \
    while(bsecs > 1024 && bidx < 3) { bsecs /= 1024; bidx++; }; \
    printf("%-25s %s ops/sec ",(name), commaize(count/elapsed)); \
    if (elapsed/count*1e9 < 10) printf("%.1f ns/op ", elapsed/count*1e9); \
    else printf("%.0f ns/op ", elapsed/count*1e9); \
    printf("%.2f %s/s\n", bsecs, bsize[bidx]);


void bench_json_valid() {
    printf("=== bench_json_valid\n");
    printf("# Validate json using 'json_valid'.\n");
    for (int i = 0; i < sizeof(names)/sizeof(char *); i++){
        // if (strcmp(names[i], "twitter.json") != 0) continue;
        long len = 0;
        char *json = readtestfile(names[i], &len);
        bench(names[i], 0.10, json_validn(json, len));
        free(json);
    }
    printf("\n");
}

void bench_json_parse() {
    printf("=== bench_json_parse\n");
    printf("# Parse json using 'json_parse'.\n");
    for (int i = 0; i < sizeof(names)/sizeof(char *); i++){
        long len = 0;
        char *json = readtestfile(names[i], &len);
        bench(names[i], 0.10, json_parsen(json, len));
        free(json);
    }
    printf("\n");
}

void bench_canada_coordinates() {
    printf("=== bench_canada_coordinates\n");
    printf("# Count the rings in a large GeoJSON file using the path\n"
           "# 'features.0.geometry.coordinates' followed by calling json_array_count().\n");
    long len = 0;
    char *json = readtestfile("canada.json", &len);
    struct json val = json_getn(json, len, "features.0.geometry.coordinates");
    const char *end = json_raw(val)+json_raw_length(val);
    size_t isize0 = end-json; // only calculate the number of bytes read.
    bench("canada.json", 0.10, {
        struct json val = json_getn(json, len, "features.0.geometry.coordinates");
        long n = json_array_count(val);
        isize = isize0;
        // printf("%d\n", n);
        // long len = json_raw_length(val);
        // printf("%.*s\n", (int)json_raw_length(val), json_raw(val));        
    });
    free(json);
    printf("\n");
}

void bench_canada_count_coordinates() {
}

void bench_twitter_status() {
    printf("=== bench_twitter_status\n");
    printf("# Get the id of the 50th tweet using the path 'statuses.50.id'.\n");
    long len = 0;
    char *json = readtestfile("twitter.json", &len);

    bench("twitter.json", 0.10, {
        struct json val = json_getn(json, len, "statuses.50.id");
        const char *end = json_raw(val)+json_raw_length(val);
        isize = end-json; // only calculate the number of bytes read.
        // long len = json_raw_length(val);
        // printf("%.*s\n", (int)json_raw_length(val), json_raw(val));
    });
    free(json);
    printf("\n");
}

void bench_twitter_count_statues() {
    printf("=== bench_twitter_count_statuses\n");
    printf("# Get a value near the end of a using the path 'search_metadata.count'.\n");
    long len = 0;
    char *json = readtestfile("twitter.json", &len);

    bench("twitter.json", 0.10, {
        struct json val = json_getn(json, len, "search_metadata.count");
        const char *end = json_raw(val)+json_raw_length(val);
        isize = end-json; // only calculate the number of bytes read.
    });
    free(json);
    printf("\n");
}

int main(int argc, char **argv) {
    nameless_tests = true;
    do_test(bench_json_valid);
    // do_test(bench_json_parse);
    do_test(bench_canada_coordinates);
    do_test(bench_canada_count_coordinates);
    do_test(bench_twitter_status);
    do_test(bench_twitter_count_statues);
    return 0;
}