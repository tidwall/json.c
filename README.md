# json.c

A fast little JSON library for C.

## Features

- **Fast.** Handles gigabytes of JSON per second.
- **Lightweight.** Small codebase that compiles quickly.
- **Portable.** Uses standard C.
- Parses and validates UTF-8. 
- Access elements using a dot path syntax.
- Works directly on the raw JSON.
- Uses no memory allocations. 

The primary goal of this library is to provide a set of functions for
quickly cherry-picking elements from large raw JSON documents, without 
additional allocations.

## Example 

```c
#include "json.h"

char json_str[] = "{\"name\":{\"first\":\"Janet\",\"last\":\"Prichard\"},\"age\":47}";

// Get the last name by its path.
// A path is a series of keys separated by a dot.
struct json value = json_get(json_str, "name.last");
char last_name[64];
json_string_copy(value, last_name, sizeof(last_name));

// Get the age as an integer.
int64_t age = json_int64(json_get(json_str, "age"));

printf("%s %d\n", last_name, age);
```

This will print:

```
Prichard 47
```

## API

```c

// json_valid returns true if the input is valid json data.
bool json_valid(const char *json_str);

// json_parse parses the input data and returns a json value.
//
// This function expects that the json is well-formed, and does not validate.
// Invalid json is safe and will not cause program errors, but it may return
// unexpected results. If you are consuming JSON from an unpredictable source
// then you may want to use the 'json_valid' function first.
// 
// It's important to note that the resulting 'struct json' is backed by the
// same memory as json_str, thus a 'struct json' cannot outlive the original
// json_str. Attempting to use  any of the json_*(struct json...) functions on
// the result, beyond the lifetime of the originating 'json_str', then it will
// likely result in a memory fault or some other type of undefined corruption.
struct json json_parse(const char *json_str);

// json_first returns the json's first child.
//
// For objects, the key of the first value is returned. For arrays, the first
// child value is returned. If the input json has no children, or is not an
// object or array, then a non-existent json value is returned.
struct json json_first(struct json json);

// json_next returns the json's next child. 
//
// This typically follows a 'json_first' call, and is used to iterate over the 
// children in an object or array. For arrays, each 'json_next' call will
// return a the next value. For objects, each 'json_next' call alternates over
// value, key, value  key, value, etc. If there are no more children, then a
// non-existent json value is returned.
struct json json_next(struct json json);

// json_exists checks for the existence of json. 
bool json_exists(struct json json);

// json_type returns the json's type. 
//
// Returns one of the following: JSON_NULL, JSON_STRING, JSON_NUMBER,
// JSON_TRUE, JSON_FALSE, JSON_OBJECT, JSON_ARRAY.
// For non-existent json, JSON_NULL is returned.
enum json_type json_type(struct json json);

// json_raw returns the start of the raw json data.
// 
// This function may return NULL if the input is non-existent. Also there is
// no guarentee that this data will be null-terminated C string. If you want to
// retain this data for longer that the data from the original 'json_parse'
// call then you should copy it first, such as:
// 
//    size_t len = json_raw_length(json);
//    char *raw = malloc(len+1);
//    memcpy(raw, json_raw(json), len);
//    raw[len] = '\0';
//
const char *json_raw(struct json json);

// json_raw_length returns the length of the raw json data.
size_t json_raw_length(struct json json);

// Compare a json's raw data with a C string.
//
// This function performs a binary comparison of the characters. It compares
// each character, one-by-one, of both strings. If they are equal to each
// other, it continues until the characters differ or until the end of the JSON
// or a terminating null-character in the C string is reached.
// Returns -1, 0, +1 for less-than, equal-to, greater-than.
int json_raw_compare(struct json json, const char *str);

// Compares a json string to the provided C string.
//
// This function performs a binary comparison of the characters. It compares
// each character, one-by-one, of both strings. If they are equal to each
// other, it continues until the characters differ or until the end of the JSON
// or a terminating null-character in the C string is reached.
//
// If the JSON type is JSON_STRING, then each character that needs to be 
// unescaped is done so in a streaming manner. ie. without the additional
// buffering or memory allocations. Of those characters, the ones that are
// non-ascii codepoints are encoded into their UTF-8 representation, of which
// those bytes are then compared to the C string.
// 
// If the JSON type is not a JSON_STRING, then it performs a binary comparison
// of the raw JSON to the C string.
int json_string_compare(struct json json, const char *str);

// json_string_length returns the number of characters needed to represent the
// JSON as a UTF-8 string.
size_t json_string_length(struct json json);

// json_string_copy copies a json string into the provided C string buffer.
//
// Returns the number of characters, not including the null-terminator, needed
// to store the JSON into the C string buffer.
// If the returned length is greater than nbytes-1, then only a parital copy
// occurred, for example:
//    
//    char buf[64];
//    size_t len = json_string_copy(json, str, sizeof(str));
//    if (len > sizeof(str)-1) {
//        // ... copy did not complete ...
//    }
//
size_t json_string_copy(struct json json, char *str, size_t nbytes);

// json_array_get returns the child json element at index. This is to be used
// on json with the type JSON_ARRAY.
struct json json_array_get(struct json json, size_t index);

// json_array_count returns the number of elements in a json array.
size_t json_array_count(struct json json);

// json_object_get returns the json value for its key. This is to be used on
// json with the type JSON_OBJECT.
struct json json_object_get(struct json json, const char *key);

// json_get return finds json at the provide path.
//
// The path is a series of C strings followed by zero-terminator.
//
//    json_get(doc, "name", "last", 0);
//
struct json json_get(const char *json_str, const char *path);

// json_double returns a json's double value.
double json_double(struct json json);

// json_int returns a json's int value.
int json_int(struct json json);

// json_int64 returns a json's int64 value.
int64_t json_int64(struct json json);

// json_uint64 returns a json's uint64 value.
uint64_t json_uint64(struct json json);

// json_bool returns a json's boolean value.
bool json_bool(struct json json);

// json_escape is a utility function for converting a C string into a valid
// json string. 
//
// Returns the number of characters, not including the null-terminator, needed
// to store the escaped JSON string in the C string buffer.
// If the returned length is greater than n-1, then only a parital copy
// occurred. In other words the 
size_t json_escape(const char *str,char *escaped, size_t n); 

```

Many of the functions above accept null-terminated C strings as inputs.

If you happen to know the length of the string ahead of time then it'll
likely be more efficient to provide it using one of the following.

```c
bool json_validn(const char *str, size_t len);
struct json json_parsen(const char *str, size_t len);
int json_raw_comparen(struct json json, const char *str, size_t len);
int json_string_comparen(struct json json, const char *str, size_t len);
struct json json_object_getn(struct json json, const char *key, size_t len);
size_t json_escapen(const char *str, size_t len, char *esc, size_t n); 
struct json json_object_getn(struct json json, const char *key, size_t len);
struct json json_getn(const char *json_str, size_t len, const char *path);
```

## Path syntax

A path is a series of keys separated by a dot. 

```c
json_get(json_str, "name");
json_get(json_str, "user.id");
```

For now, the syntax is limited to very basic key names containing only 
alphanumeric characters, underscores, and dashes.
If you need to access keys that have a broader range of characters you can use
the `json_object_get` and `json_array_get` functions.

```c
json_str = "{\"notes\": {\"special.info\": \"it's nice outside\"}";

json = json_parse(json_str); 
json = json_object_get(json, "notes"); 
json = json_object_get(json, "special.info");
```

## Performance

```python
$ tests/run.sh bench

=== bench_json_valid
# Validate json using 'json_valid'.
apache_builds.json        10,950 ops/sec 91317 ns/op 1.30 GB/s
canada.json               749 ops/sec 1334125 ns/op 1.57 GB/s
citm_catalog.json         1,141 ops/sec 875958 ns/op 1.84 GB/s
github_events.json        41,962 ops/sec 23831 ns/op 2.55 GB/s
gsoc-2018.json            962 ops/sec 1039020 ns/op 2.98 GB/s
instruments.json          9,154 ops/sec 109240 ns/op 1.88 GB/s
marine_ik.json            451 ops/sec 2213240 ns/op 1.26 GB/s
mesh.json                 1,851 ops/sec 540179 ns/op 1.25 GB/s
mesh.pretty.json          888 ops/sec 1125056 ns/op 1.31 GB/s
numbers.json              11,608 ops/sec 86143 ns/op 1.62 GB/s
random.json               2,653 ops/sec 376926 ns/op 1.26 GB/s
twitter.json              3,360 ops/sec 297547 ns/op 2.07 GB/s
twitterescaped.json       3,418 ops/sec 292560 ns/op 1.79 GB/s
update-center.json        4,179 ops/sec 239250 ns/op 2.08 GB/s

=== bench_canada_coordinates
# Count the rings in a large GeoJSON file using the path
# 'features.0.geometry.coordinates' followed by calling json_array_count().
canada.json               1,729 ops/sec 578189 ns/op 3.63 GB/s

=== bench_twitter_status
# Get the id of the 50th tweet using the path 'statuses.50.id'.
twitter.json              12,547 ops/sec 79698 ns/op 3.98 GB/s

=== bench_twitter_count_statuses
# Get a value near the end of a using the path 'search_metadata.count'.
twitter.json              6,661 ops/sec 150119 ns/op 4.11 GB/s
```

